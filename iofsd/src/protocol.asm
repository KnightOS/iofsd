#include "kernel.inc"

current_thread:
    .db 0

init_io:
    pcall(getCurrentThreadId)
    kld((current_thread), a)
    kld((resume_callback + 1), a)

    kld(ix, packet_callback)
    ld b, 4 \ ld a, 0x51
    pcall(ioRegisterHandler)
    ld ix, 0
    ret

packet_callback:
    kld(de, incoming_packet)
    push de
        ld bc, 4
        ldir
    pop de
    ex de, hl
    inc hl \ inc hl
    ld c, (hl) \ inc hl
    ld b, (hl)
    ld hl, 0
    pcall(cpHLBC)
    jr z, .ready
    ; Bulk packet
    kld(hl, buffer)
    kld(ix, .ready)
    ret
.ready:
    ld a, 1
    kld((ready), a)
    kld(a, (current_thread))
    pcall(resumeThread)
    ld bc, 0
    ret

resume_callback:
    ld a, 0
    pcall(resumeThread)
    ret

incoming_packet:
    .block 4
buffer:
    .block 256

send_buffer:
    pcall(ioSendBuffer)
    ret z
    jr send_buffer

handle_packet:
    kld(a, (ready))
    or a
    ret z
    xor a
    kld((ready), a)

    kld(a, (incoming_packet))
    cp 0x51
    ret nz
    xor a
    kld((incoming_packet), a)

    kld(a, (incoming_packet + 1))
    ld b, a
    kld(hl, packet_handlers)
.loop:
    ld a, (hl)
    inc hl
    cp 0xFF
    jr z, .none
    cp b
    jr z, .found
    jr .loop
.none:
    kld(hl, .none)
    jr .done
.done:
    ret
.found:
    ld e, (hl)
    inc hl
    ld d, (hl)
    ex de, hl
    kld(bc, 0)
    add hl, bc
    kld(de, .done)
    push de
    push hl
    ret
