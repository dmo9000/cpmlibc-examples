.module lldetect
.globl _lldetect 

_lldetect:

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
 in a, (#0xf0)
 ld h, #0
 ld l, a
 pop af
 ret

