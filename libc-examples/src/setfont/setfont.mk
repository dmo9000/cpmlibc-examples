example-setfont: $(BIN_DIR)/setfont.arf $(BIN_DIR)/setfont.com

clean-setfont:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/setfont.com:	tools $(SETFONT_OBJS) $(BIN_DIR)/setfont.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/setfont

SETFONT_DIR = src/setfont
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
SETFONT_OBJS = $(SETFONT_DIR)/setfont.rel

SETFONT_ARF = $(BIN_DIR)/setfont.arf

$(BIN_DIR)/setfont.ihx:	$(SETFONT_OBJS)
	@echo "Linking setfont.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/setfont.arf


$(BIN_DIR)/setfont.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/setfont/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/setfont.arf
	echo "src/setfont/setfont.rel" >> $(BIN_DIR)/setfont.arf
	sed -i '/^$$/d' $(BIN_DIR)/setfont.arf

$(SETFONT_DIR)/setfont.rel: $(SETFONT_DIR)/setfont.c
	$(CCC) $(ECCC_FLAGS) -o $(SETFONT_DIR)/setfont.rel $<

