example-malltest: $(BIN_DIR)/malltest.arf $(BIN_DIR)/malltest.com

clean-malltest:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/malltest.com:	tools $(MALLTEST_OBJS) $(BIN_DIR)/malltest.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/malltest

MALLTEST_DIR = src/malltest
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
MALLTEST_OBJS = $(MALLTEST_DIR)/malltest.rel

MALLTEST_ARF = $(BIN_DIR)/malltest.arf

$(BIN_DIR)/malltest.ihx:	$(MALLTEST_OBJS)
	@echo "Linking malltest.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/malltest.arf


$(BIN_DIR)/malltest.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/malltest/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/malltest.arf
	echo "src/malltest/malltest.rel" >> $(BIN_DIR)/malltest.arf
	sed -i '/^$$/d' $(BIN_DIR)/malltest.arf

$(MALLTEST_DIR)/malltest.rel: $(MALLTEST_DIR)/malltest.c
	$(CCC) $(ECCC_FLAGS) -o $(MALLTEST_DIR)/malltest.rel $<

