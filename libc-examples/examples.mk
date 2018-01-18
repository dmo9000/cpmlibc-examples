REPLACE_TAG=REPLACE_ME_PLEASE
CPM_LIBC_DIR=/usr/share/sdcc/lib/z80cpm
EXAMPLES_INCLUDE_DIR=-I/usr/share/sdcc/z80cpm/include

examples-install: build-drive9 build-driveB

examples:   $(LIBC_DIR)/link-static.arf			\
			 example-cat example-fdump example-getopt \
	    	 example-cls example-bogomips example-tstansi example-format example-fstat \
   		     example-cvi example-readrip example-getfont example-setfont example-telnet \
	  	     example-iotest example-malltest example-cpstest example-randtest example-lifegame \
    	     example-testlibc util-writerip fonts-8x8 iotestdata package-oem

diskimages:	 package-oem build-driveB build-drive9	
	
			
package-oem:
	arc a oempkg.arc bin/cls.com bin/format.com bin/cat.com bin/bogomips.com bin/cpstest.com bin/fdump.com \
    bin/vi.com bin/telnet.com bin/lzf.com bin/fstat.com bin/osz.com bin/iotest.com bin/malltest.com \
    bin/lifegame.com bin/testlibc.com \
    1>/dev/null
	arc a oemimage.arc pngrip/*.rip bin/readrip.com 1>/dev/null 
	ls -l *.arc
	[ -d /usr/local/bbsd/data ] && sudo cp *.arc /usr/local/bbsd/data  

push-oem:
	cp *.arc ../z80oem
	cd ../z80oem
	git commit -a -m `cat ../osz/build-id`
	git push

build-drive9:
	cd diskbuild && make clean && make

build-driveB:
	dd if=/dev/zero ibs=256256 count=1 | tr "\000" "\345" > ~/src/z80pack/z80pack-1.34/cpmsim/disks/driveb.dsk

iotestdata:
	head -c 786432 < /dev/zero | tr '\0' '\101' > testdata/testfile.bin

liblzf:
	cd libs/liblzf-3.6-orig && make
	# cp libs/liblzf-3.6-orig/bin/lzf.com bin/lzf.com

fonts-8x8:
	cd fonts/8x8 && make pngrip && make unpack-fonts && make 

util-writerip: 
	cd pngrip && make



examples-clean:
	rm -f $(LIBC_DIR)/*.com
	rm -f $(LIBC_DIR)/*.ihx
	rm -f $(LIBC_DIR)/*.arf


$(LIBC_DIR)/link-static.arf:   
	$(QUIET)$(ECHO) Generating link-static.arf
	$(QUIET)$(ECHO) -mjx > $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) -i $(BIN_DIR)/$(REPLACE_TAG).ihx >> $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) -l z80 >> $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) $(CPM_LIBC_DIR)/cpm0.rel >> $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) $(CPM_LIBC_DIR)/libc.a >> $(LIBC_DIR)/link-static.arf
	$(QUIET)$(ECHO) -e >> $(LIBC_DIR)/link-static.arf


