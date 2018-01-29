example-nyancat: $(BIN_DIR)/nyancat.arf $(BIN_DIR)/nyancat.com

clean-nyancat:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/nyancat.com:	tools $(NYANCAT_OBJS) $(BIN_DIR)/nyancat.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/nyancat

NYANCAT_DIR = src/nyancat
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
NYANCAT_OBJS = $(NYANCAT_DIR)/nyancat.rel

NYANCAT_ARF = $(BIN_DIR)/nyancat.arf

$(BIN_DIR)/nyancat.ihx:	$(NYANCAT_OBJS)
	@echo "Linking nyancat.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/nyancat.arf


$(BIN_DIR)/nyancat.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/nyancat/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/nyancat.arf
	echo "src/nyancat/nyancat.rel" >> $(BIN_DIR)/nyancat.arf
	sed -i '/^$$/d' $(BIN_DIR)/nyancat.arf

$(NYANCAT_DIR)/nyancat.rel: $(NYANCAT_DIR)/nyancat.c
	$(CCC) $(ECCC_FLAGS) -o $(NYANCAT_DIR)/nyancat.rel $<

