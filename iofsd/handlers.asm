packet_handlers:
    .db 0x01
    .dw handle_ping
    .db 0xFF

handle_ping:
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    di \ kcall(send_buffer)
    pcall(suspendCurrentThread)
    ret
.response:
    .db 0x52, 0x01, 0x00, 0x00
