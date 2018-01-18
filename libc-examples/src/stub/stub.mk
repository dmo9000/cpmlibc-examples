example-stub: $(BIN_DIR)/stub.arf $(BIN_DIR)/stub.com

clean-stub:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/stub.com:	tools $(STUB_OBJS) $(BIN_DIR)/stub.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/stub

STUB_DIR = src/stub
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
STUB_OBJS = $(STUB_DIR)/stub.rel

STUB_ARF = $(BIN_DIR)/stub.arf

$(BIN_DIR)/stub.ihx:	$(STUB_OBJS)
	@echo "Linking stub.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/stub.arf


$(BIN_DIR)/stub.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/stub/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/stub.arf
	echo "src/stub/stub.rel" >> $(BIN_DIR)/stub.arf
	sed -i '/^$$/d' $(BIN_DIR)/stub.arf

$(STUB_DIR)/stub.rel: $(STUB_DIR)/stub.c
	$(CCC) $(ECCC_FLAGS) -o $(STUB_DIR)/stub.rel $<

