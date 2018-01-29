#!/bin/sh
rm -f osz/isr0.rel
sdasz80 -o osz/isr0.s
sdcc   -mz80  --code-loc 0x0 --data-loc 0x90 -o osz/bios.ihx --no-std-crt0 --nostdlib --nostdinc osz/isr0.rel 
