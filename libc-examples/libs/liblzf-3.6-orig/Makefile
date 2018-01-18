VERSION = 3.6

prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
includedir = ${prefix}/include

CLD = sdcc-sdldz80
LIBC_DIR = ../../libc
CC = sdcc-sdcc
CPPFLAGS = -I. 
CFLAGS = -I../../src/include -mz80 -D__SDCC__=1
LDFLAGS = 
RANLIB = ranlib
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644

all: Makefile bin/lzf.ihx ../../bin/lzf.com ../../bin/lzf.com

bin/lzf.com: bin/lzf.ihx
	../../lbin/load bin/lzf

../../bin/lzf.com: bin/lzf.com
	cp bin/lzf.com ../../bin/lzf.com

clean:
	rm -f *.rel *.asm *.lst *.sym *.noi *.lk *.map *.arf *.a lzf bench 
	rm -f bin/*

lzf_c.rel: lzf_c.c lzfP.h
	$(CC) -I../../src/include -D__SDCC__=1 -mz80 -c lzf_c.c

lzf_d.rel: lzf_d.c lzfP.h
	$(CC) -I../../src/include -D__SDCC__=1 -mz80 -c lzf_d.c

lzf.rel: lzf.c
	$(CC) -I../../src/include -D__SDCC__=1 -mz80 -c lzf.c

lzf: lzf.rel lzf_c.rel lzf_d.rel

lzfP.h: lzf.h config.h

#liblzf.a: lzf_c.rel lzf_d.rel
#	rm -f $@
#	$(AR) rc $@ $^
#	$(RANLIB) $@

install: all
	$(INSTALL) -d $(bindir)
	$(INSTALL) -m 755 lzf $(bindir)
	$(INSTALL) -d $(includedir)
	$(INSTALL_DATA) lzf.h $(includedir)
	$(INSTALL) -d $(libdir)
	$(INSTALL_DATA) liblzf.a $(libdir)

dist:
	mkdir liblzf-$(VERSION)
	tar c LICENSE README Makefile.in config.h.in \
              configure configure.ac install-sh \
	      cs/README cs/CLZF.cs \
              lzf.h lzfP.h lzf_c.c lzf_d.c \
              crc32.h lzf.c Changes \
              | tar xpC liblzf-$(VERSION)
	-chown -R root.root liblzf-$(VERSION)
	chmod -R u=rwX,go=rX liblzf-$(VERSION)
	tar cvf - liblzf-$(VERSION) | gzip -9 >liblzf-$(VERSION).tar.gz
	rm -rf liblzf-$(VERSION)
	ls -l liblzf-$(VERSION).tar.gz

Makefile: Makefile.in
	./config.status

bench: Makefile liblzf.a bench.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -g -o bench bench.c -L. -llzf


bin/lzf.ihx: lzf.rel lzf_c.rel lzf_d.rel lzf.arf
	@echo "Linking lzf.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf lzf.arf


lzf.arf: $(LIBC_DIR)/libc.arf
	sed 's/REPLACE_ME_PLEASE/lzf/' $(LIBC_DIR)/libc.arf > lzf.arf 
	echo "lzf.rel" >> lzf.arf
	echo "lzf_c.rel" >> lzf.arf
	echo "lzf_d.rel" >> lzf.arf
	sed -i '/^$$/d' lzf.arf
	sed -i 's/^libc/..\/..\/libc/g' lzf.arf

