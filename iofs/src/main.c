#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <glib.h>
#include "protocol.h"
#include "operations.h"

CableHandle *handle;

void kiofs_abort(int err) {
	if (handle != NULL) {
		ticables_cable_close(handle);
		ticables_handle_del(handle);
		ticables_library_exit();
	}

	if (err == 1234) {
		fprintf(stderr, "Unexpected response\n");
		exit(1);
	}

	char *message;
	ticables_error_get(err, &message);
	fprintf(stderr, "Error %d: %s", err, message);
	g_free(message);
	exit(1);
}

int main(int argc, char **argv) {
	ticables_library_init();

	handle = ticables_handle_new(CABLE_SLV, PORT_1);
	if (handle == NULL) {
		return 1;
	}

	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_handle_show(handle);

	int err = ticables_cable_open(handle);
	if (err) kiofs_abort(err);

	if (argc > 1) {
		if (!strcmp("ping", argv[1])) {
			send_ping(handle, &err);
			if (!err) {
				printf("pong");
			}
		}
		if (!strcmp("ls", argv[1])) {
			send_ls(handle, argv[2], &err);
		}
		if (!strcmp("recv", argv[1])) {
			recv_file(handle, argv[2], argv[3], &err);
		}
	}

	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	return 0;
}
