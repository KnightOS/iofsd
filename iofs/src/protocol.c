#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include "protocol.h"

#define MID 0x51

int send_packet(CableHandle *handle, const kpacket_t *packet) {
	uint8_t header[4];
	header[0] = MID;
	header[1] = packet->cmd;
	header[2] = packet->len & 0xFF;
	header[3] = (packet->len >> 8) & 0xFF;
	int err = ticables_cable_send(handle, header, sizeof(header));
	if (err) return err;
	if (packet->len > 0) {
		err = ticables_cable_send(handle, packet->payload, packet->len);
	}
	return err;
}

int recv_packet(CableHandle *handle, kpacket_t *packet) {
	uint8_t header[4];
	int err = ticables_cable_recv(handle, header, sizeof(header));
	if (err) return err;
	packet->cmd = header[1];
	packet->len = (header[3] << 8) | header[2];
	packet->payload = NULL;
	if (packet->len != 0) {
		packet->payload = malloc(packet->len);
		err = ticables_cable_recv(handle, packet->payload, packet->len);
	}
	return err;
}
