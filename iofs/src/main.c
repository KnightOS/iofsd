#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <glib.h>
#include "protocol.h"

void kiofs_abort(int err) {
	char *message;
	ticables_error_get(err, &message);
	fprintf(stderr, "Error %d: %s", err, message);
	g_free(message);
	exit(1);
}

int main(int argc, char **argv) {
	ticables_library_init();

	CableHandle *handle = ticables_handle_new(CABLE_SLV, PORT_1);
	if (handle == NULL) {
		return 1;
	}

	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_handle_show(handle);

	int err = ticables_cable_open(handle);

	kpacket_t packet;
	packet.cmd = CMD_PING;
	packet.len = 0;
	packet.payload = NULL;

	err = send_packet(handle, &packet);
	if (err) kiofs_abort(err);

	err = recv_packet(handle, &packet);
	if (err) kiofs_abort(err);

	if (packet.cmd == CMD_PING) {
		printf("pong\n");
	} else {
		printf("unexpected response\n");
	}

	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	return 0;
}
