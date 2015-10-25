#include "kernel.inc"
    .db "KEXC"
    .db KEXC_ENTRY_POINT
    .dw start
    .db KEXC_STACK_SIZE
    .dw 20
    .db KEXC_HEADER_END
start:
    kcall(init_io)

packet_loop:
    pcall(suspendCurrentThread)
    kcall(handle_packet)
    jr packet_loop

ready:
    .db 0

#include "handlers.asm"
#include "protocol.asm"
