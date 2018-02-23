.module llclock
.globl _llclock 

; FIXME: this needs a haircut, it doesn't need all register preservation crap I don't think

_llclock:

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
 in a, (#0xf1)
; ld l, a
; in a, (#0xf2)
; ld h, a
 pop af
 ret

