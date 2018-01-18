example-tstansi: $(BIN_DIR)/tstansi.arf $(BIN_DIR)/tstansi.com

clean-tstansi:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/tstansi.com:	tools $(TSTANSI_OBJS) $(BIN_DIR)/tstansi.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/tstansi

TSTANSI_DIR = src/tstansi
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
TSTANSI_OBJS = $(TSTANSI_DIR)/tstansi.rel

TSTANSI_ARF = $(BIN_DIR)/tstansi.arf

$(BIN_DIR)/tstansi.ihx:	$(TSTANSI_OBJS)
	@echo "Linking tstansi.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/tstansi.arf


$(BIN_DIR)/tstansi.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/tstansi/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/tstansi.arf
	echo "src/tstansi/tstansi.rel" >> $(BIN_DIR)/tstansi.arf
	sed -i '/^$$/d' $(BIN_DIR)/tstansi.arf

$(TSTANSI_DIR)/tstansi.rel: $(TSTANSI_DIR)/tstansi.c
	$(CCC) $(ECCC_FLAGS) -o $(TSTANSI_DIR)/tstansi.rel $<

