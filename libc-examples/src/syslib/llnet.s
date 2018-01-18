.module llnet
.globl _llnet 

_llnet:

 push af
 push bc
 push de
 push hl
 push ix
 push iy
 exx
 ex af, af
 push af
 push bc
 push de
 push hl



 pop hl
 pop de
 pop bc
 pop af
 ex af, af
 exx
 pop iy
 pop ix
 pop hl
 pop de
 pop bc

;; character "T" here tells the system controller that 
;; the DMA buffer contains command(s) for the TCP offload engine (TOE)

 ld a, #054
 out (#0xf0), a
 ld h, #0
 ld l, a
 pop af
 ret

