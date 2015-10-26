#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include "protocol.h"
#include "operations.h"

int send_ping(CableHandle *handle, int *err) {
	kpacket_t packet;
	packet.cmd = CMD_PING;
	packet.len = 0;
	packet.payload = NULL;

	*err = send_packet(handle, &packet);
	if (*err) return *err;

	*err = recv_packet(handle, &packet);

	if (!*err && packet.cmd != CMD_PING) {
		*err = 1234;
	}
	return *err;
}

kpacket_t send_ls(CableHandle *handle, const char *dir, int *err) {
	kpacket_t packet;
	packet.cmd = CMD_LS;
	packet.len = strlen(dir) + 1;
	packet.payload = (uint8_t *)dir;

	*err = send_packet(handle, &packet);
	if (*err) return packet;

	*err = recv_packet(handle, &packet);
	if (*err) return packet;

	int len = (int)packet.payload[0];
	fprintf(stderr, "%d entries\n", len);

	int i;
	for (i = 0; i < len; ++i) {
		*err = recv_packet(handle, &packet);
		if (*err) return packet;
		if (packet.len < 2) continue;
		if (packet.payload[0] == 0x7F) { // file
			fprintf(stderr, "%s\n", packet.payload + 1);
		} else if (packet.payload[0] == 0xBF) { // dir
			fprintf(stderr, "%s/\n", packet.payload + 1);
		} else if (packet.payload[0] == 0xDF) { // symlink
			fprintf(stderr, "%s (link)\n", packet.payload + 1);
		}
	}

	return packet;
}
