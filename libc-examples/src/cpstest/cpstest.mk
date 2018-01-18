example-cpstest: $(BIN_DIR)/cpstest.arf $(BIN_DIR)/cpstest.com

clean-cpstest:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/cpstest.com:	tools $(CPSTEST_OBJS) $(BIN_DIR)/cpstest.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/cpstest

CPSTEST_DIR = src/cpstest
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
CPSTEST_OBJS = $(CPSTEST_DIR)/cpstest.rel

CPSTEST_ARF = $(BIN_DIR)/cpstest.arf

$(BIN_DIR)/cpstest.ihx:	$(CPSTEST_OBJS)
	@echo "Linking cpstest.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/cpstest.arf


$(BIN_DIR)/cpstest.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/cpstest/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/cpstest.arf
	echo "src/cpstest/cpstest.rel" >> $(BIN_DIR)/cpstest.arf
	sed -i '/^$$/d' $(BIN_DIR)/cpstest.arf

$(CPSTEST_DIR)/cpstest.rel: $(CPSTEST_DIR)/cpstest.c
	$(CCC) $(ECCC_FLAGS) -o $(CPSTEST_DIR)/cpstest.rel $<

