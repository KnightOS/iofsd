#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>

int send_ping(CableHandle *handle, int *err);
kpacket_t send_ls(CableHandle *handle, const char *dir, int *err);
kpacket_t recv_file(CableHandle *handle, const char *path, const char *output, int *err);
