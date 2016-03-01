#include "kernel.inc"
#include "corelib.inc"

draw_ui:
    ld a, 0b00000100
    kld(hl, .window_title)
    corelib(drawWindow)

    ld e, 0
    ld l, 7
    ld c, 96
    ld b, 49
    pcall(rectAND) ; Clear the area for chat

    ld de, 0x0032
    ld hl, 0x5F32
    pcall(drawLine)
    ret
.window_title:
    .asciiz "iofsd"

draw_message:
    kld(de, (.cursor))
    ld a, 0x32
    cp e
    kcall(z, shift_line)

    xor a
    ld bc, 0x6132
    corelib(wordWrap)

    pcall(newline)
    kld((.cursor), de)
    ret
.cursor:
    .dw 0x0008

shift_line:
    push af
    push de
    push hl
    push bc
        push iy \ pop hl
        ld bc, 8 * 12
        add hl, bc
        ex de, hl ; DE is destination (row 8)
        push iy \ pop hl
        ld bc, (8 + 6) * 12
        add hl, bc ; HL is source (row 8 + 6)
        ld bc, ((64 - 15) - (8 + 6)) * 12
        ldir

        ld e, 0
        ld l, 0x32 - 6
        ld c, 96
        ld b, 6
        pcall(rectAND) ; Clear the area for new message
    pop bc
    pop hl
    pop de

    kld(de, (cursor@draw_message))
    ld a, -6 \ add a, e \ ld e, a
    kld((cursor@draw_message), de)

    pop af
    ret
