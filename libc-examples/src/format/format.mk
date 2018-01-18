example-format: $(BIN_DIR)/format.arf $(BIN_DIR)/format.com

clean-format:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/format.com:	tools $(FORMAT_OBJS) $(BIN_DIR)/format.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/format

FORMAT_DIR = src/format
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
FORMAT_OBJS = $(FORMAT_DIR)/format.rel

FORMAT_ARF = $(BIN_DIR)/format.arf

$(BIN_DIR)/format.ihx:	$(FORMAT_OBJS)
	@echo "Linking format.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/format.arf


$(BIN_DIR)/format.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/format/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/format.arf
	echo "src/format/format.rel" >> $(BIN_DIR)/format.arf
	sed -i '/^$$/d' $(BIN_DIR)/format.arf

$(FORMAT_DIR)/format.rel: $(FORMAT_DIR)/format.c
	$(CCC) $(ECCC_FLAGS) -o $(FORMAT_DIR)/format.rel $<

