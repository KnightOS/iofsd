#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <glib.h>
#include "protocol.h"
#include "operations.h"

void kiofs_abort(int err) {
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

	CableHandle *handle = ticables_handle_new(CABLE_SLV, PORT_1);
	if (handle == NULL) {
		return 1;
	}

	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_handle_show(handle);

	int err = ticables_cable_open(handle);

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
	}
	if (err) kiofs_abort(err);


	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	return 0;
}
