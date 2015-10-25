handle_ping:
    kld(hl, .log)
    kcall(log)
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    di \ kcall(send_buffer)
    pcall(suspendCurrentThread)
    kld(hl, .log_pong)
    kcall(log)
    ret
.response:
    .db 0x52, 0x01, 0x00, 0x00
.log:
    .db "<- ping", 0
.log_pong:
    .db "-> pong", 0

handle_list_dir:
    kld(hl, .log)
    kcall(log)
    kld(hl, buffer)
    kcall(log)
    kld(hl, .response)
    ld bc, 4
    kld(ix, resume_callback)
    di \ kcall(send_buffer)
    pcall(suspendCurrentThread)
    kld(hl, .log_resp)
    kcall(log)
    ret
.response:
    .db 0x52, 0x81, 0x00, 0x00
.log:
    .db "<- ls", 0
.log_resp:
    .db "-> ack", 0

resume_callback:
    ld a, 0
    pcall(resumeThread)
    ret
