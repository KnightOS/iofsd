#define FUSE_USE_VERSION 30

#include <tilp2/ticables.h>
#include <glib.h>
#include <fuse.h>
#include <errno.h>
#include "protocol.h"
#include "operations.h"

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

FILE *logfd;
CableHandle *handle;
extern int socketfd;

#define HOST 0x51

static int iofs_error(int err) {
	char *message;
	ticables_error_get(err, &message);
	fprintf(logfd, "ticables error %s", message);
	fflush(logfd);
	g_free(message);
	return -EIO;
}

static void *iofs_init(struct fuse_conn_info *conn) {
	logfd = fopen("/tmp/iofs.log", "a+");
	fprintf(logfd, "Initialized iofs\n");
	fflush(logfd);

	/*const char *socket_path = "/tmp/z80e.sock";
	struct sockaddr_un addr;
	if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Failed to open socket\n");
		return NULL;
	}
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path));
	addr.sun_path[sizeof(addr.sun_path) - 1] = 0;
	int l = sizeof(addr.sun_family) + strlen(addr.sun_path);
	if (connect(socketfd, (struct sockaddr *)&addr, l) == -1) {
		fprintf(stderr, "Unable to connect to %s", socket_path);
		return NULL;
	}*/

	ticables_library_init();
	handle = ticables_handle_new(CABLE_SLV, PORT_1);
	ticables_options_set_timeout(handle, 15);
	ticables_options_set_delay(handle, 10);
	ticables_cable_open(handle);
	return NULL;
}

static void iofs_destroy(void *unused) {
	ticables_cable_close(handle);
	ticables_handle_del(handle);
	ticables_library_exit();
	handle = NULL;
}

static int iofs_getattr(const char *path, struct stat *stbuf) {
	// TODO: Query calc for file existence
	
	fprintf(logfd, "getattr %s\n", path);
	stbuf->st_mode = 0777;

	return 0;
}

static int iofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi) {
	int err;
	kpacket_t packet;
	packet.host = HOST;
	packet.cmd = CMD_LS;
	packet.len = strlen(path) + 1;
	packet.payload = (uint8_t *)path;

	fprintf(logfd, "Reading %s\n", path);
	fflush(logfd);
	err = send_packet(handle, &packet);
	if (err) return iofs_error(err);

	err = recv_packet(handle, &packet);
	if (err) return iofs_error(err);

	int len = (int)packet.payload[0];
	fprintf(logfd, "%d entries\n", len);
	fflush(logfd);
	int i;
	for (i = 0; i < len; ++i) {
		err = recv_packet(handle, &packet);
		if (err) return iofs_error(err);
		if (packet.len < 2) continue;

		fprintf(logfd, "%s\n", packet.payload + 1);
		fflush(logfd);

		struct stat st;
		memset(&st, 0, sizeof(st));
		switch (packet.payload[0]) {
		case 0xBF:
			st.st_mode = S_IFDIR;
			break;
		case 0xDF:
			st.st_mode = S_IFLNK;
			break;
		case 0x7F:
		default:
			st.st_mode = S_IFREG;
			break;
		}
		st.st_mode |= 0777;
		if (filler(buf, (const char *)(packet.payload + 1), &st, 0)) {
			break;
		}
	}

	return 0;
}

struct fuse_operations iofs_oper = {
	.init		= iofs_init,
	.destroy 	= iofs_destroy,
	.getattr	= iofs_getattr,
	.readdir	= iofs_readdir,
};
