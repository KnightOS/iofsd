#include "kernel.inc"
    .db "KEXC"
    .db KEXC_ENTRY_POINT
    .dw start
    .db KEXC_STACK_SIZE
    .dw 20
    .db KEXC_NAME
    .dw name
    .db KEXC_HEADER_END
name:
    .db "iofsd", 0
start:
    pcall(getLcdLock)
    pcall(getKeypadLock)
    pcall(allocScreenBuffer)
    kld((screen), iy)
    pcall(clearBuffer)

    kcall(init_io)
    kld(hl, ready_str)
    kcall(log)

packet_loop:
    ;pcall(suspendCurrentThread)
    kcall(handle_packet)
    jr packet_loop

ready:
    .db 0

ready_str:
    .db "Ready", 0

log:
    push de
    push bc
    push af
    push hl
        ld b, 0
        kld(de, (.cur))
        pcall(drawStr)
        pcall(newline)
        pcall(fastCopy)
        ld a, 64
        cp e
        jr nz, .ret
        ld e, 58
        kld((.cur), de)
        push iy \ pop hl
        push iy \ pop de
        ld bc, 12 * 6
        add hl, bc
        ld bc, 768 - (12 * 6)
        ldir
        ld c, 96
        ld b, 6
        ld e, 0
        ld l, 58
        pcall(rectAND)
    pop hl
    pop af
    pop bc
    pop de
    ret
.ret:
        kld((.cur), de)
    pop hl
    pop af
    pop bc
    pop de
    ret
.cur:
    .dw 0x0004

screen:
    .dw 0

#include "handlers.asm"
#include "protocol.asm"
