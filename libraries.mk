LIBC_DIR = libc

# Here begins the actual creation of destination files

OBJS = $(LIBC_DIR)/cpm0.rel $(LIBC_DIR)/vprintf.rel $(LIBC_DIR)/cprintf.rel $(LIBC_DIR)/cstdio.rel $(LIBC_DIR)/cpmbdos.rel $(LIBC_DIR)/ansi_term.rel $(LIBC_DIR)/cpm_sysfunc.rel \
            $(LIBC_DIR)/strtol.rel $(LIBC_DIR)/fcntl.rel $(LIBC_DIR)/errno.rel $(LIBC_DIR)/string.rel $(LIBC_DIR)/ctype.rel $(LIBC_DIR)/random.rel \
            $(LIBC_DIR)/hw_common.rel $(LIBC_DIR)/hw_modprn02.rel $(LIBC_DIR)/getopt.rel $(LIBC_DIR)/lldetect.rel $(LIBC_DIR)/llclock.rel $(LIBC_DIR)/llcommand.rel 	\
			$(LIBC_DIR)/heap.rel $(LIBC_DIR)/malloc.rel $(LIBC_DIR)/time.rel $(LIBC_DIR)/tcp.rel $(LIBC_DIR)/grx.rel $(LIBC_DIR)/llgrx.rel $(LIBC_DIR)/llnet.rel


libraries: libc/libc.a

libc/libc.a: $(OBJS)
	sdcc-sdar -rc $(LIBC_DIR)/libc.a `find libc -name "*.rel" ! -name "cpm0.rel" | sort` 

$(LIBC_DIR)/llcommand.rel: src/syslib/llcommand.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llcommand.rel src/syslib/llcommand.s

$(LIBC_DIR)/lldetect.rel: src/syslib/lldetect.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/lldetect.rel src/syslib/lldetect.s

$(LIBC_DIR)/llclock.rel: src/syslib/llclock.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llclock.rel src/syslib/llclock.s

$(LIBC_DIR)/llgrx.rel: src/syslib/llgrx.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llgrx.rel src/syslib/llgrx.s

$(LIBC_DIR)/llnet.rel: src/syslib/llnet.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llnet.rel src/syslib/llnet.s

#libraries: $(OBJS)
#	sdcc-sdar -rc $(LIBC_DIR)/libc.a `find libc -name "*.rel" ! -name "cpm0.rel" | sort` 

libraries-install: $(LIBC_DIR)/cpm0.rel $(LIBC_DIR)/libc.a 
	sudo rm -rf /usr/share/sdcc/lib/z80cpm
	sudo rm -rf /usr/share/sdcc/lib/z80cpm
	sudo mkdir -p /usr/share/sdcc/lib/z80cpm
	sudo cp $(LIBC_DIR)/cpm0.rel $(LIBC_DIR)/libc.a /usr/share/sdcc/lib/z80cpm
	sudo mkdir -p /usr/share/sdcc/z80cpm/include
	sudo cp -rfp src/include/* /usr/share/sdcc/z80cpm/include

libraries-clean:
	rm -f $(LIBC_DIR)/*.rel $(LIBC_DIR)/*.asm $(LIBC_DIR)/*.lst $(LIBC_DIR)/*.map $(LIBC_DIR)/*.noi $(LIBC_DIR)/*.sym $(LIBC_DIR)/*.a

$(LIBC_DIR)/heap.rel: $(SYSLIB_SRC_DIR)/heap.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/heap.c

$(LIBC_DIR)/grx.rel: $(SYSLIB_SRC_DIR)/grx.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/grx.c

$(LIBC_DIR)/tcp.rel: $(SYSLIB_SRC_DIR)/tcp.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/tcp.c

$(LIBC_DIR)/ctype.rel: $(SYSLIB_SRC_DIR)/ctype.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/ctype.c

$(LIBC_DIR)/string.rel: $(SYSLIB_SRC_DIR)/string.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/string.c

$(LIBC_DIR)/errno.rel: $(SYSLIB_SRC_DIR)/errno.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/errno.c

$(LIBC_DIR)/fcntl.rel: $(SYSLIB_SRC_DIR)/fcntl.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/fcntl.c

$(LIBC_DIR)/cprintf.rel: $(SYSLIB_SRC_DIR)/cprintf.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/cprintf.c

$(LIBC_DIR)/cstdio.rel: $(SYSLIB_SRC_DIR)/cstdio.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/cstdio.c

$(LIBC_DIR)/malloc.rel: $(SYSLIB_SRC_DIR)/malloc.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/malloc.c

$(LIBC_DIR)/random.rel: $(SYSLIB_SRC_DIR)/random.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/random.c

$(LIBC_DIR)/vprintf.rel: $(SYSLIB_SRC_DIR)/vprintf.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/vprintf.c

$(LIBC_DIR)/strtol.rel: $(SYSLIB_SRC_DIR)/strtol.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/strtol.c

$(LIBC_DIR)/getopt.rel: $(SYSLIB_SRC_DIR)/getopt.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/getopt.c

$(LIBC_DIR)/cpm_sysfunc.rel: $(SYSLIB_SRC_DIR)/cpm_sysfunc.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/cpm_sysfunc.c

$(LIBC_DIR)/ansi_term.rel: $(SYSLIB_SRC_DIR)/ansi_term.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/ansi_term.c

$(LIBC_DIR)/time.rel: $(SYSLIB_SRC_DIR)/time.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SYSLIB_SRC_DIR)/time.c

$(LIBC_DIR)/cpmbdos.rel:	$(SRC_DIR)/cpm/cpmbdos.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(SRC_DIR)/cpm/cpmbdos.c

$(LIBC_DIR)/hw_modprn02.rel: $(HWLIB_SRC_DIR)/modprn02/hw_modprn02.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(HWLIB_SRC_DIR)/modprn02/hw_modprn02.c

$(LIBC_DIR)/hw_common.rel: $(HWLIB_SRC_DIR)/common/hw_common.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR) $(HWLIB_SRC_DIR)/common/hw_common.c


# Build CP/M-80 Command File Structure files
$(LIBC_DIR)/cpm0.rel: $(CPM_SRC_DIR)/cpm0.s
	$(CAS) $(CAS_FLAGS) $(CPM_SRC_DIR)/cpm0.rel $(CPM_SRC_DIR)/cpm0.s
	$(QUIET)$(COPY) $(CPM_SRC_DIR)/cpm0.rel $(LIBC_DIR)
	$(QUIET)$(COPY) $(CPM_SRC_DIR)/cpm0.lst $(LIBC_DIR)
