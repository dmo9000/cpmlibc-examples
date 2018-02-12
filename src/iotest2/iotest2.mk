example-iotest2: $(BIN_DIR)/iotest2.arf $(BIN_DIR)/iotest2.com

clean-iotest2:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/iotest2.com:	tools $(IOTEST2_OBJS) $(BIN_DIR)/iotest2.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/iotest2

IOTEST2_DIR = src/iotest2
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
IOTEST2_OBJS = $(IOTEST2_DIR)/iotest2.rel

IOTEST2_ARF = $(BIN_DIR)/iotest2.arf

$(BIN_DIR)/iotest2.ihx:	$(IOTEST2_OBJS)
	@echo "Linking iotest2.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/iotest2.arf


$(BIN_DIR)/iotest2.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/iotest2/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/iotest2.arf
	echo "src/iotest2/iotest2.rel" >> $(BIN_DIR)/iotest2.arf
	sed -i '/^$$/d' $(BIN_DIR)/iotest2.arf

$(IOTEST2_DIR)/iotest2.rel: $(IOTEST2_DIR)/iotest2.c
	$(CCC) $(ECCC_FLAGS) -o $(IOTEST2_DIR)/iotest2.rel $<

