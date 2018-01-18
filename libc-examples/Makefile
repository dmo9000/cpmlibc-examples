MAXALLOCS=50000
# cat /proc/cpuinfo | grep "^processor" | wc -l
#MAXALLOCS=100000

include common.mk

CFLAGS = --std-c99 --fverbose-asm 
CLD_FLAGS=-k /usr/share/sdcc/lib/z80/
MIRRORS=/media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3 /media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3 /media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3
IMGDIRS := $(shell ls -1  /media/sf_OSZ/systems/ 2>/dev/null )
THISBUILDID := $(shell cat build-id )

all: 	bump-build tools libraries 

bump-build: build-id build-id.h
	./build-id.sh	
	@echo "+++ build-id for this build is ${THISBUILDID}"

clean-arf:
	find . -name "*.arf" -print -exec rm -f {} \;

clean:	tools-clean libraries-clean examples-clean 
	rm -f $(BIN_DIR)/*
	rm -f $(CAT_DIR)/*.asm $(CAT_DIR)/*.sym $(CAT_DIR)/*.lst $(CAT_DIR)/*.rel
	rm -f $(FDUMP_DIR)/*.asm $(FDUMP_DIR)/*.sym $(FDUMP_DIR)/*.lst $(FDUMP_DIR)/*.rel
	find . -name "*.rel" ! -name "isr0.rel" -exec rm -f {} \;
	find . -name "*.sym" -exec rm -f {} \;
	find . -name "*.lst" -exec rm -f {} \;
	find . -name "*.asm" -exec rm -f {} \;
	rm -f oem*.arc
	cd libs/liblzf-3.6-orig && make clean
	cd pngrip && make clean
	cd fonts/8x8 && make clean


install: libraries-install 

include tools.mk # Local tools
include libraries.mk # Libraries
include examples.mk # Examples
include src/cat/cat.mk
include src/cls/cls.mk
include src/getopt/getopt.mk
include src/fdump/fdump.mk
include src/bogomips/bogomips.mk
include src/tstansi/tstansi.mk
include src/format/format.mk
include src/fstat/fstat.mk
include src/cvi/cvi.mk
include src/readrip/readrip.mk
include src/getfont/getfont.mk
include src/setfont/setfont.mk
include src/telnet/telnet.mk
include src/iotest/iotest.mk
include src/malltest/malltest.mk
include src/cpstest/cpstest.mk
include src/randtest/randtest.mk
include src/lifegame/lifegame.mk
include src/testlibc/testlibc.mk
include esrc/bios/bios.mk
include esrc/osz/osz.mk
