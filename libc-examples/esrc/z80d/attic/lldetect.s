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
 ld  hl,#0x00ff
 pop af
 ret

