example-getfont: $(BIN_DIR)/getfont.arf $(BIN_DIR)/getfont.com

clean-getfont:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/getfont.com:	tools $(GETFONT_OBJS) $(BIN_DIR)/getfont.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/getfont

GETFONT_DIR = src/getfont
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
GETFONT_OBJS = $(GETFONT_DIR)/getfont.rel

GETFONT_ARF = $(BIN_DIR)/getfont.arf

$(BIN_DIR)/getfont.ihx:	$(GETFONT_OBJS)
	@echo "Linking getfont.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/getfont.arf


$(BIN_DIR)/getfont.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/getfont/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/getfont.arf
	echo "src/getfont/getfont.rel" >> $(BIN_DIR)/getfont.arf
	sed -i '/^$$/d' $(BIN_DIR)/getfont.arf

$(GETFONT_DIR)/getfont.rel: $(GETFONT_DIR)/getfont.c
	$(CCC) $(ECCC_FLAGS) -o $(GETFONT_DIR)/getfont.rel $<

