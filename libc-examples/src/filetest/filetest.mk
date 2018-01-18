example-filetest: $(BIN_DIR)/filetest.arf $(BIN_DIR)/filetest.com

clean-filetest:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/filetest.com:	tools $(FILETEST_OBJS) $(BIN_DIR)/filetest.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/filetest

FILETEST_DIR = src/filetest
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
FILETEST_OBJS = $(FILETEST_DIR)/filetest.rel

FILETEST_ARF = $(BIN_DIR)/filetest.arf

$(BIN_DIR)/filetest.ihx:	$(FILETEST_OBJS)
	@echo "Linking filetest.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/filetest.arf


$(BIN_DIR)/filetest.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/filetest/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/filetest.arf
	echo "src/filetest/filetest.rel" >> $(BIN_DIR)/filetest.arf
	sed -i '/^$$/d' $(BIN_DIR)/filetest.arf

$(FILETEST_DIR)/filetest.rel: $(FILETEST_DIR)/filetest.c
	$(CCC) $(ECCC_FLAGS) -o $(FILETEST_DIR)/filetest.rel $<

