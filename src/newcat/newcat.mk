example-newcat: $(BIN_DIR)/newcat.arf $(BIN_DIR)/newcat.com

clean-newcat:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/newcat.com:	tools $(NEWCAT_OBJS) $(BIN_DIR)/newcat.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/newcat

NEWCAT_DIR = src/newcat
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
NEWCAT_OBJS = $(NEWCAT_DIR)/newcat.rel

NEWCAT_ARF = $(BIN_DIR)/newcat.arf

$(BIN_DIR)/newcat.ihx:	$(NEWCAT_OBJS)
	@echo "Linking newcat.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/newcat.arf


$(BIN_DIR)/newcat.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/newcat/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/newcat.arf
	echo "src/newcat/newcat.rel" >> $(BIN_DIR)/newcat.arf
	sed -i '/^$$/d' $(BIN_DIR)/newcat.arf

$(NEWCAT_DIR)/newcat.rel: $(NEWCAT_DIR)/newcat.c
	$(CCC) $(ECCC_FLAGS) -o $(NEWCAT_DIR)/newcat.rel $<

