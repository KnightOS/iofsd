#include "kernel.inc"
    .db "KEXC"
    .db KEXC_ENTRY_POINT
    .dw main
    .db KEXC_STACK_SIZE
    .dw 30
    .db KEXC_HEADER_END
