example-debug: $(BIN_DIR)/debug.arf $(BIN_DIR)/debug.com

clean-debug:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/debug.com:	tools $(DEBUG_OBJS) $(BIN_DIR)/debug.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/debug

DEBUG_DIR = src/debug
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
DEBUG_OBJS = $(DEBUG_DIR)/debug.rel

DEBUG_ARF = $(BIN_DIR)/debug.arf

$(BIN_DIR)/debug.ihx:	$(DEBUG_OBJS)
	@echo "Linking debug.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/debug.arf


$(BIN_DIR)/debug.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/debug/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/debug.arf
	echo "src/debug/debug.rel" >> $(BIN_DIR)/debug.arf
	sed -i '/^$$/d' $(BIN_DIR)/debug.arf

$(DEBUG_DIR)/debug.rel: $(DEBUG_DIR)/debug.c
	$(CCC) $(ECCC_FLAGS) -o $(DEBUG_DIR)/debug.rel $<

