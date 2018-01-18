.module llcommand
.globl _llcommand 

_llcommand:

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
 out (#0xf0), a
 ld h, #0
 ld l, a
 pop af
 ret

