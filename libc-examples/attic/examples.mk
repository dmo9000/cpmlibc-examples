REPLACE_TAG=REPLACE_ME_PLEASE

examples:	$(LIBC_DIR)/libc.arf $(LIBC_DIR)/libc-mini.arf $(LIBC_DIR)/libc-tiny.arf

examples-clean:
	rm -f $(LIBC_DIR)/*.com
	rm -f $(LIBC_DIR)/*.ihx
	rm -f $(LIBC_DIR)/*.arf


$(LIBC_DIR)/libc-static.arf:   
	$(QUIET)$(ECHO) Generating libc-static.arf
	$(QUIET)$(ECHO) -mjx > $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) -i $(BIN_DIR)/$(REPLACE_TAG).ihx >> $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) -l z80 >> $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm0.rel >> $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/libc.a >> $(LIBC_DIR)/libc-static.arf
	$(QUIET)$(ECHO) -e >> $(LIBC_DIR)/libc-static.arf


$(LIBC_DIR)/libc-tiny.arf:   
	$(QUIET)$(ECHO) Generating libc-tiny.arf
	$(QUIET)$(ECHO) -mjx > $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) -i $(BIN_DIR)/$(REPLACE_TAG).ihx >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) -l z80 >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm0.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cprintf.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm_sysfunc.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ansi_term.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/getopt.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llcommand.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/lldetect.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llclock.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ctype.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/heap.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llnet.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/tcp.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/time.rel >> $(LIBC_DIR)/libc-tiny.arf
	$(QUIET)$(ECHO) -e >> $(LIBC_DIR)/libc-tiny.arf


$(LIBC_DIR)/libc-mini.arf:   
	$(QUIET)$(ECHO) Generating libc-mini.arf
	$(QUIET)$(ECHO) -mjx > $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) -i $(BIN_DIR)/$(REPLACE_TAG).ihx >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) -l z80 >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm0.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpmbdos.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cprintf.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cstdio.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/strtol.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/string.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/errno.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm_sysfunc.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ansi_term.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/hw_common.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/hw_modprn02.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/getopt.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llcommand.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/lldetect.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llclock.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ctype.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/heap.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llnet.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/tcp.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/time.rel >> $(LIBC_DIR)/libc-mini.arf
	$(QUIET)$(ECHO) -e >> $(LIBC_DIR)/libc-mini.arf


$(LIBC_DIR)/libc.arf:	
	$(QUIET)$(ECHO) Generating libc.arf
	$(QUIET)$(ECHO) -mjx > $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) -i $(BIN_DIR)/$(REPLACE_TAG).ihx >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) -k $(COMPILER_LIBS) >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) -l z80 >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm0.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpmbdos.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/fcntl.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cprintf.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cstdio.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/strtol.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/string.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/errno.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/cpm_sysfunc.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ansi_term.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/hw_common.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/hw_modprn02.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/getopt.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llcommand.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/lldetect.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llclock.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/ctype.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/heap.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/grx.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llgrx.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/llnet.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/tcp.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/time.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/malloc.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) $(LIBC_DIR)/random.rel >> $(LIBC_DIR)/libc.arf
	$(QUIET)$(ECHO) -e >> $(LIBC_DIR)/libc.arf
