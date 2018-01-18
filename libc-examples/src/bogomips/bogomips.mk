example-bogomips: $(BIN_DIR)/bogomips.arf $(BIN_DIR)/bogomips.com

clean-bogomips:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/bogomips.com:	tools $(BOGOMIPS_OBJS) $(BIN_DIR)/bogomips.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/bogomips

BOGOMIPS_DIR = src/bogomips
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
BOGOMIPS_OBJS = $(BOGOMIPS_DIR)/bogomips.rel

BOGOMIPS_ARF = $(BIN_DIR)/bogomips.arf

$(BIN_DIR)/bogomips.ihx:	$(BOGOMIPS_OBJS)
	@echo "Linking bogomips.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/bogomips.arf


$(BIN_DIR)/bogomips.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/bogomips/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/bogomips.arf
	echo "src/bogomips/bogomips.rel" >> $(BIN_DIR)/bogomips.arf
	sed -i '/^$$/d' $(BIN_DIR)/bogomips.arf

$(BOGOMIPS_DIR)/bogomips.rel: $(BOGOMIPS_DIR)/bogomips.c
	$(CCC) $(ECCC_FLAGS) -o $(BOGOMIPS_DIR)/bogomips.rel $<

