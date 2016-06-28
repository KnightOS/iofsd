#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <tilp2/ticalcs.h>
#include "protocol.h"

#define HOST_KERNEL 0x5F
#define CMD_ACK 0x56
#define CMD_ERR 0x5A

uint16_t checksum(uint8_t *data, uint16_t len) {
	uint16_t sum = 0;
	while (len--) {
		sum += data[len];
	}
	return sum;
}

int send_packet(CableHandle *handle, const kpacket_t *packet) {
	uint8_t header[4];
	bool done = false;
	uint16_t sum = checksum(packet->payload, packet->len);
	int err = 1;
	header[0] = packet->host;
	header[1] = packet->cmd;
	header[2] = packet->len & 0xFF;
	header[3] = (packet->len >> 8) & 0xFF;
	kpacket_t ack;
	while (!done) {
		if (packet->cmd == CMD_ACK && packet->host == HOST_KERNEL) {
			printf("Sending ACK\n");
		} else if (packet->cmd == CMD_ERR && packet->host == HOST_KERNEL) {
			printf("Sending ERR\n");
		} else {
			printf("Sending %02X to host %02X (%d bytes)\n", packet->cmd, packet->host, packet->len);
		}
		err = ticables_cable_send(handle, header, sizeof(header));
		if (err) break;
		if (packet->len > 0) {
			err = ticables_cable_send(handle, packet->payload, packet->len);
			if (err) break;
			err = ticables_cable_send(handle, (uint8_t *)&sum, 2);
		}
		if (packet->host != HOST_KERNEL || (packet->cmd != CMD_ACK && packet->cmd != CMD_ERR)) {
			printf("Reciving response\n");
			err = recv_packet(handle, &ack);
			if (ack.host == HOST_KERNEL && (ack.cmd == CMD_ACK || ack.cmd == CMD_ERR)) {
				if (ack.cmd == CMD_ACK) {
					done = true;
				} else {
					printf("Calculator requested retry\n");
				}
			} else {
				// whelp
				err = 1;
				break;
			}
		} else {
			done = true;
		}
	}
	return err;
}

int send_err(CableHandle *handle) {
	kpacket_t packet = { .cmd = CMD_ERR, .len = 0x0000, .host = HOST_KERNEL };
	return send_packet(handle, &packet);
}

int send_ack(CableHandle *handle) {
	kpacket_t packet = { .cmd = CMD_ACK, .len = 0x0000, .host = HOST_KERNEL };
	return send_packet(handle, &packet);
}

int recv_packet(CableHandle *handle, kpacket_t *packet) {
	uint8_t header[4];
	bool done = false;
	uint16_t sum;
	int err = 1;
	while (!done) {
		printf("Receiving header\n");
		err = ticables_cable_recv(handle, header, sizeof(header));
		if (err) break;
		printf("Received header\n");
		packet->host = header[0];
		packet->cmd = header[1];
		packet->len = (header[3] << 8) | header[2];
		packet->payload = NULL;
		if (packet->len != 0) {
			printf("Receiving %d bytes\n", packet->len);
			packet->payload = malloc(packet->len);
			err = ticables_cable_recv(handle, packet->payload, packet->len);
			if (err) break;
			printf("Received %d bytes\n", packet->len);
			err = ticables_cable_recv(handle, (uint8_t*)&sum, 2);
			if (checksum(packet->payload, packet->len) != sum) {
				free(packet->payload);
				err = send_err(handle);
				if (err) break;
			} else {
				done = true;
				err = send_ack(handle);
			}
		} else {
			done = true;
		}
	}
	return err;
}
