#define FUSE_USE_VERSION 30

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>
#include <glib.h>
#include <fuse.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.h"
#include "operations.h"

CableHandle *handle;
extern struct fuse_operations iofs_oper;
extern FILE *logfd;
int socketfd;

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
	if (argc > 1 && !strcmp("mount", argv[1])) {
		return fuse_main(argc - 1, argv + 1, &iofs_oper, NULL);
	}

	logfd = fopen("/tmp/iofs.log", "a+");

	ticables_library_init();

	handle = ticables_handle_new(CABLE_SLV, PORT_1);
	if (handle == NULL) {
		return 1;
	}

	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_handle_show(handle);

	int err;
	ticables_cable_open(handle);
	if (err) kiofs_abort(err);

	//const char *socket_path = "/tmp/z80e.sock";
	//struct sockaddr_un addr;
	//if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	//	fprintf(stderr, "Failed to open socket\n");
	//	return 1;
	//}
	//addr.sun_family = AF_UNIX;
	//strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path));
	//addr.sun_path[sizeof(addr.sun_path) - 1] = 0;
	//int l = sizeof(addr.sun_family) + strlen(addr.sun_path);
	//if (connect(socketfd, (struct sockaddr *)&addr, l) == -1) {
	//	fprintf(stderr, "Unable to connect to %s", socket_path);
	//	return 1;
	//}

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

	if (handle) {
		ticables_cable_close(handle);
		ticables_handle_del(handle);
		ticables_library_exit();
	}
	return err;
}
