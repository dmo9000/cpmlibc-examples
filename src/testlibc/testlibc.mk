example-testlibc: $(BIN_DIR)/testlibc.arf $(BIN_DIR)/testlibc.com

clean-testlibc:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/testlibc.com:	tools $(TESTLIBC_OBJS) $(BIN_DIR)/testlibc.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/testlibc

TESTLIBC_DIR = src/testlibc
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
TESTLIBC_OBJS = $(TESTLIBC_DIR)/testlibc.rel
TESTLIBC_ARF = $(BIN_DIR)/testlibc.arf

$(BIN_DIR)/testlibc.ihx:	$(TESTLIBC_OBJS)
	@echo "Linking testlibc.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/testlibc.arf


$(BIN_DIR)/testlibc.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/testlibc/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/testlibc.arf
	echo "src/testlibc/testlibc.rel" >> $(BIN_DIR)/testlibc.arf
	sed -i '/^$$/d' $(BIN_DIR)/testlibc.arf

$(TESTLIBC_DIR)/testlibc.rel: $(TESTLIBC_DIR)/testlibc.c
	$(CCC) $(ECCC_FLAGS) -o $(TESTLIBC_DIR)/testlibc.rel $<

