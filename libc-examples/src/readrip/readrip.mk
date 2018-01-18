example-readrip: $(BIN_DIR)/readrip.arf $(BIN_DIR)/readrip.com

clean-readrip:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/readrip.com:	tools $(READRIP_OBJS) $(BIN_DIR)/readrip.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/readrip

READRIP_DIR = src/readrip
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
READRIP_OBJS = $(READRIP_DIR)/readrip.rel

READRIP_ARF = $(BIN_DIR)/readrip.arf

$(BIN_DIR)/readrip.ihx:	$(READRIP_OBJS)
	@echo "Linking readrip.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/readrip.arf


$(BIN_DIR)/readrip.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/readrip/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/readrip.arf
	echo "src/readrip/readrip.rel" >> $(BIN_DIR)/readrip.arf
	sed -i '/^$$/d' $(BIN_DIR)/readrip.arf

$(READRIP_DIR)/readrip.rel: $(READRIP_DIR)/readrip.c
	$(CCC) $(ECCC_FLAGS) -o $(READRIP_DIR)/readrip.rel $<

