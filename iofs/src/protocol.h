#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tilp2/ticables.h>

typedef struct {
    uint8_t cmd;
    uint8_t host;
    uint16_t len;
    uint8_t *payload;
} kpacket_t;

#define CMD_PING 0x01
#define CMD_LS 0x81

int send_packet(CableHandle *handle, const kpacket_t *packet);
int recv_packet(CableHandle *handle, kpacket_t *packet);
