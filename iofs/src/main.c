#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <glib.h>

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
	if (err) {
		kiofs_abort(err);
	}

	uint8_t recv_buf[256];

	uint8_t ping_buf[] = { 0x51, 0x01, 0x00, 0x00 };

	err = ticables_cable_send(handle, ping_buf, sizeof(ping_buf));
	if (err) {
		kiofs_abort(err);
	}
	printf("-> ping\n");

	err = ticables_cable_recv(handle, recv_buf, 4);
	if (err) {
		kiofs_abort(err);
	}
	printf("<- ");
	int i;
	for (i = 0; i < 4; ++i) {
		printf("%02X", recv_buf[i]);
	}
	printf("\n");

	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	return 0;
}
