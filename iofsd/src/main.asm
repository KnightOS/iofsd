#include "kernel.inc"
#include "corelib.inc"

main:
    kld(de, .corelib)
    pcall(loadLibrary)

    kcall(init_io)

    pcall(getLcdLock)
    pcall(getKeypadLock)
    pcall(allocScreenBuffer)

    kcall(draw_ui)

    kld(hl, .ready_message)
    kcall(draw_message)

    pcall(flushKeys)

    kjp(packet_loop)
.ready_message:
    .db "iofsd ready.", 0
.corelib:
    .db "/lib/core", 0

packet_loop:
    pcall(getKey)
    cp kMODE
    ret z
    pcall(fastCopy)
    kcall(handle_packet)
    jr packet_loop

ready:
    .db 0
