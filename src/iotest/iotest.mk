example-iotest: $(BIN_DIR)/iotest.arf $(BIN_DIR)/iotest.com

clean-iotest:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/iotest.com:	tools $(IOTEST_OBJS) $(BIN_DIR)/iotest.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/iotest

IOTEST_DIR = src/iotest
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
IOTEST_OBJS = $(IOTEST_DIR)/iotest.rel

IOTEST_ARF = $(BIN_DIR)/iotest.arf

$(BIN_DIR)/iotest.ihx:	$(IOTEST_OBJS)
	@echo "Linking iotest.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/iotest.arf


$(BIN_DIR)/iotest.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/iotest/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/iotest.arf
	echo "src/iotest/iotest.rel" >> $(BIN_DIR)/iotest.arf
	sed -i '/^$$/d' $(BIN_DIR)/iotest.arf

$(IOTEST_DIR)/iotest.rel: $(IOTEST_DIR)/iotest.c
	$(CCC) $(ECCC_FLAGS) -o $(IOTEST_DIR)/iotest.rel $<

