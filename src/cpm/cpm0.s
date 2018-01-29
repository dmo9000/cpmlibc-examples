;--------------------------------------------------------------------------
;  cpm0.s - Generic cpm0.s for a Z80 CP/M-80 v2.2 Application
;  Copyright (C) 2011, Douglas Goodall All Rights Reserved.
;--------------------------------------------------------------------------

    .globl	_main
    .globl _rtargc
    .globl __cpm_sysfunc_init
		.globl _stktop
		.globl _heapaddr
	  .area	_CODE
    .ds 0x0100

init:
	;; Define an adequate stack   
		ld hl, #0xe000
		ld e,l
		ld d,h
		inc de
		ld (hl),#0xe5
		ld bc, #0x400
		ldir

		ld	sp, #_stktop
    ;; ld sp,#32767 
    ;; HI as we can go I think
   ;; ld sp,#0xe3ff
    ;; ld sp, #0xdfff

        ;; Initialise global variables
    call    gsinit
    call    __cpm_sysfunc_init

    ld hl,#0x0050
    push hl
    ld hl,(_rtargc)
    push hl

	;; Call the C main routine
	call	_main

	ld	c,#0
	call	5

	;; Ordering of segments for the linker.
	.area	_TPA

	.area	_HOME
	.area	_CODE
  .area _GSINIT
  .area _GSFINAL
	.area	_DATA
_rtargc::
    .dw 0xFFFF
_rtargv::
    .ds 16

	.area	_STACK
	.ds	256
_stktop::
				.dw #0xE3FF
        .area   _GSINIT
gsinit::

        .area   _GSFINAL
        ret
	.db	0xe5

				.area _HEAP
_heapaddr::
				.dw 0x0000
;;;;;;;;;;;;;;;;
; eof - cpm0.s ;
;;;;;;;;;;;;;;;;

