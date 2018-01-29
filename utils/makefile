
CC = gcc
CFLAGS = -g -Wall

all:	dstat10.com dstat11.com dstat12.com \
	dstat80.com dstat86.cmd 

%.com:	%.rel
	zxcc link80 $<

%.rel:	%.zsm
	zxcc m80 -=$<

%.rel:	%.asm
	zxcc rmac $<

bin2cmd:	bin2cmd.c

dstat86.cmd:	dstat86.bin bin2cmd
	./bin2cmd $< $@

dstat86.bin:	dstat86.a86
	nasm -f bin -o $@ $<
