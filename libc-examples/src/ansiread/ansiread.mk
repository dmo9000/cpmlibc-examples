example-ansiread: $(BIN_DIR)/ansiread.arf $(BIN_DIR)/ansiread.com

clean-ansiread:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/ansiread.com:	tools $(ANSIREAD_OBJS) $(BIN_DIR)/ansiread.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/ansiread

ANSIREAD_DIR = src/ansiread
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
ANSIREAD_OBJS = $(ANSIREAD_DIR)/ansiread.rel

ANSIREAD_ARF = $(BIN_DIR)/ansiread.arf

$(BIN_DIR)/ansiread.ihx:	$(ANSIREAD_OBJS)
	@echo "Linking ansiread.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/ansiread.arf


$(BIN_DIR)/ansiread.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/ansiread/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/ansiread.arf
	echo "src/ansiread/ansiread.rel" >> $(BIN_DIR)/ansiread.arf
	sed -i '/^$$/d' $(BIN_DIR)/ansiread.arf

$(ANSIREAD_DIR)/ansiread.rel: $(ANSIREAD_DIR)/ansiread.c
	$(CCC) $(ECCC_FLAGS) -o $(ANSIREAD_DIR)/ansiread.rel $<

