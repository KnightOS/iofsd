#include "kernel.inc"

packet_handlers:
    .db 0x01
    .dw handle_ping
    .db 0x81
    .dw handle_ls
    .db 0xFF

handle_ping:
    kld(hl, .in_msg)
    kcall(draw_message)
    kld(hl, .out_msg)
    kcall(draw_message)

    ld hl, 0 \ ld bc, 0
    ld de, 0x0152 ; pong
    kld(ix, resume_callback)
    kjp(send_buffer)
.in_msg:
    .db "<- PING\n", 0
.out_msg:
    .db "-> PONG ", 0

send_not_found:
    kld(hl, .out_msg)
    kcall(draw_message)

    ld hl, 0 \ ld bc, 0
    ld de, 0x4152 ; not found
    kld(ix, resume_callback)
    kjp(send_buffer)
.out_msg:
    .db "-> NOT FOUND ", 0

send_oom:
    kld(hl, .out_msg)
    kcall(draw_message)

    ld hl, 0 \ ld bc, 0
    ld de, 0x4252
    kld(ix, resume_callback)
    di \ kcall(send_buffer)
    pcall(suspendCurrentThread)
    ret
.out_msg:
    .db "-> OOM ", 0

handle_ls:
    kld(hl, .in_ls_msg)
    kcall(draw_message)
    kld(hl, buffer)
    kcall(draw_message)
    kld(hl, .out_ready_msg_newline)
    kcall(draw_message)

    kld(de, buffer)
    pcall(directoryExists)
    jp nz, send_not_found

    ld bc, 0x100
    pcall(malloc)
    kld((.buffer), ix)
    jp nz, send_oom
    kld(hl, .list_callback)
    pcall(listDirectory)
    kld(a, (.total))
    kld((.ready_response), a) ; num entries
    
    kld(hl, .out_ready_msg)
    kcall(draw_message)
    
    kld(hl, .ready_response)
    ld bc, 1
    ld de, 0x8152 ; READY
    kld(ix, resume_callback)
    kcall(send_buffer) ; Send preamble

    kld(a, (.total))
    ld b, a
    kld(ix, (.buffer))
.loop:
    push bc
        ld l, (ix) \ inc ix
        ld h, (ix) \ inc ix
        push ix
            pcall(strlen)
            inc bc
            kld(de, buffer)
            push bc
            push hl
                ldir
            pop ix
            pop bc
            pcall(free)
            kld(hl, .out_entry_msg)
            kcall(draw_message)
            kld(hl, buffer)
            inc hl
            kcall(draw_message)
            dec hl
            ld de, 0x8252 ; ENTRY
            kld(ix, resume_callback)
            kcall(send_buffer)
        pop ix
    pop bc
    djnz .loop

    kld(hl, .ls_done_msg)
    kcall(draw_message)

    kld(ix, (.buffer))
    pcall(free)
    xor a
    kld((.total), a)
    ret
.list_callback:
    push de
    push hl
    push bc
    push af
        ld hl, kernelGarbage

        pcall(strlen)
        inc bc \ inc bc
        push ix
            pcall(malloc)
            ld (ix), a
            push ix \ pop de
            inc de
            dec bc
            ldir
            push ix \ pop hl
        pop ix
        ld (ix), l \ inc ix
        ld (ix), h \ inc ix

        kld(hl, .total)
        inc (hl)
    pop af
    pop bc
    pop hl
    pop de
    ret
.buffer:
    .dw 0
.total:
    .db 0
.ready_response:
    .db 0x00
.in_ls_msg:
    .db "<- LS ", 0
.out_ready_msg:
    .db "-> READY "
.out_ready_msg_newline:
    .db "\n", 0
.out_entry_msg:
    .db "-> ENTRY ", 0
.ls_done_msg:
    .db "-- DONE\n", 0
