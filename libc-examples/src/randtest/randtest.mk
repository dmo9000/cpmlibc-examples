example-randtest: $(BIN_DIR)/randtest.arf $(BIN_DIR)/randtest.com

clean-randtest:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/randtest.com:	tools $(RANDTEST_OBJS) $(BIN_DIR)/randtest.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/randtest

RANDTEST_DIR = src/randtest
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
RANDTEST_OBJS = $(RANDTEST_DIR)/randtest.rel

RANDTEST_ARF = $(BIN_DIR)/randtest.arf

$(BIN_DIR)/randtest.ihx:	$(RANDTEST_OBJS)
	@echo "Linking randtest.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/randtest.arf


$(BIN_DIR)/randtest.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/randtest/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/randtest.arf
	echo "src/randtest/randtest.rel" >> $(BIN_DIR)/randtest.arf
	sed -i '/^$$/d' $(BIN_DIR)/randtest.arf

$(RANDTEST_DIR)/randtest.rel: $(RANDTEST_DIR)/randtest.c
	$(CCC) $(ECCC_FLAGS) -o $(RANDTEST_DIR)/randtest.rel $<

