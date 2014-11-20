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
    .db "kiofs", 0
start:
    pcall(getLcdLock)
    pcall(getKeypadLock)
    pcall(getIOLock)
    xor a \ out (0), a

    pcall(allocScreenBuffer)
    pcall(clearBuffer)
    ld de, 0
.loop:
    kcall(getByte)
    ld a, ' '
    pcall(drawChar)
    pcall(drawHexA)
    pcall(newLine)
    pcall(fastCopy)
    pcall(getKey)
    cp kMODE
    jr nz, .loop
    ret

; TODO: Timeout
getByte:
    di
    ld c, 0 ; Return value
    ld l, 8 ; Times to loop
.idle_loop:
    in a, (0)
    bit 0, a
    jr z, .recv_0
    bit 1, a
    jr z, .recv_1
    jr .idle_loop
.recv_0:
    ld a, 0b10
    out (0), a
.recv_0_wait:
    in a, (0)
    bit 0, a
    jr z, .recv_0_wait
    kcall(.zero)
    jr .continue
.recv_1:
    ld a, 0b01
    out (0), a
.recv_1_wait:
    in a, (0)
    bit 1, a
    jr z, .recv_1_wait
    set 0, c
    kcall(.one)
    ; jr .continue
.continue:
    rrc c
    dec l
    xor a
    out (0), a
    cp l
    jr nz, .idle_loop
    ld a, c
    ei
    ret
.zero:
    ld a, '0'
    pcall(drawChar)
    pcall(fastCopy)
    ret
.one:
    ld a, '1'
    pcall(drawChar)
    pcall(fastCopy)
    ret
