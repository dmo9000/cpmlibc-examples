.module llgrx
.globl _llgrx 

_llgrx:

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

;; character "G" here tells the system controller that 
;; the DMA buffer contains command(s) for the graphics processor

 ld a, #0x47
 out (#0xf0), a
 ld h, #0
 ld l, a
 pop af
 ret

