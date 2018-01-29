example-lodepng: $(BIN_DIR)/lodepng.arf $(BIN_DIR)/lodepng.com

clean-lodepng:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/lodepng.com:	tools $(LODEPNG_OBJS) $(BIN_DIR)/lodepng.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/lodepng

LODEPNG_DIR = src/lodepng
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
LODEPNG_OBJS = $(LODEPNG_DIR)/lodepng.rel

LODEPNG_ARF = $(BIN_DIR)/lodepng.arf

$(BIN_DIR)/lodepng.ihx:	$(LODEPNG_OBJS)
	@echo "Linking lodepng.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/lodepng.arf


$(BIN_DIR)/lodepng.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/lodepng/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/lodepng.arf
	echo "src/lodepng/lodepng.rel" >> $(BIN_DIR)/lodepng.arf
	sed -i '/^$$/d' $(BIN_DIR)/lodepng.arf

$(LODEPNG_DIR)/lodepng.rel: $(LODEPNG_DIR)/lodepng.c
	$(CCC) $(ECCC_FLAGS) -o $(LODEPNG_DIR)/lodepng.rel $<

