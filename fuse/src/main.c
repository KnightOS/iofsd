#include <stdio.h>
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

	uint8_t buf[32];
	int i;
	for (i = 0; i < 32; ++i) {
		buf[i] = i + 1;
		printf("%02X ", buf[i]);
	}
	printf("\n");
	err = ticables_cable_send(handle, buf, 16);
	if (err) {
		kiofs_abort(err);
	}

	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	return 0;
}
