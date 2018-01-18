;; Generic crt0.s for a Z80
.area HEADER (ABS) 
;; Reset vector
.org 0
;;    OUT  (0x0),A
    jp 0x0100
.org 0x05
    IN   A,(0x0)
.org 0x07
    RET
.org 0x38
 ld ( previous_stack + 1 ), sp
 ld sp, (0xefff)

push af
 push bc
 push de
 push hl
 push ix
 push iy
 exx
 ex af, af'
 push af
 push bc
 push de
 push hl

;; jump to C interrupt service routine here

;; this section prints a '+' via CP/M putchar vector so you know it's running. comment for general use
;;     push    ix
;;    ld      ix,#0
;;     add     ix,sp

;;     ld      c,#2     ; Load function
;;     ld      e,#43  ; Prepare parameter in E ...
;;     call    5       ; Make BDOS call!
;;     pop     ix
;;;;;;;;;;;;;;;;;;;;;;;;;
;; this section increments timer counter 
    ld HL, (#0x00f7)
    inc HL
    ld (#0x00f7), HL
    ;; high byte then low byte
    ;;ld E,(HL)
    ;;inc HL
    ;;ld D,(HL)
    ;;inc DE

;;;;;;;;;;;;;;;;;;;;;;;;;;

;; end service routein

 pop hl
 pop de
 pop bc
 pop af
 ex af, af'
 exx
 pop iy
 pop ix
 pop hl
 pop de
 pop bc
 pop af
 ld sp, ( previous_stack + 1 )
 ei
 reti
.org 0xf0

.org 0x90
previous_stack:
.org 0xf0
;; magic number for identifying our BIOS and NULL terminator byte
;;bios_label:
;;.ascii "GUAVA"
;;.dw 0
.org 0xf7
clocktick1:
.dw 0
.org 0x100
.area _DATA (ABS)
