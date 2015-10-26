packet_handlers:
    .db 0x01
    .dw handle_ping
    .db 0x81
    .dw handle_ls
    .db 0xFF

; HL: header
; BC: length of buffer
prepare_response:
    push bc
        kld(de, incoming_packet)
        ld bc, 4
        ldir
    pop bc
    kld((incoming_packet + 2), bc)
    inc bc \ inc bc \ inc bc \ inc bc
    kld(hl, incoming_packet)
    kld(ix, resume_callback)
    ret

handle_ping:
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    kcall(send_and_suspend)
    ret
.response:
    .db 0x52, 0x01, 0x00, 0x00

send_not_found:
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    kcall(send_and_suspend)
    ret
.response:
    .db 0x52, 0x41, 0x00, 0x00

send_oom:
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    di \ kcall(send_buffer)
    pcall(suspendCurrentThread)
    ret
.response:
    .db 0x52, 0x42, 0x00, 0x00

handle_ls:
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
    kld((.ready_response + 4), a) ; num entries
    
    kld(hl, .ready_response)
    ld bc, 5
    kld(ix, resume_callback)
    kcall(send_and_suspend) ; Send preamble

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
            kld(hl, .entry_response)
            kcall(prepare_response)
            kcall(send_and_suspend)
        pop ix
    pop bc
    djnz .loop

    kld(ix, (.buffer))
    pcall(free)
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
    .db 0x52, 0x81, 0x01, 0x00, 0x00
.entry_response:
    .db 0x52, 0x82, 0x00, 0x00
