example-fdump: $(BIN_DIR)/fdump.arf $(BIN_DIR)/fdump.com

$(BIN_DIR)/fdump.com:	tools $(FDUMP_OBJS) $(BIN_DIR)/fdump.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/fdump

FDUMP_DIR = src/fdump
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
FDUMP_OBJS = $(FDUMP_DIR)/ffdump.rel $(FDUMP_DIR)/fdump.rel

FDUMP_ARF = $(BIN_DIR)/fdump.arf

$(BIN_DIR)/fdump.ihx:	$(FDUMP_OBJS)
	@echo "Linking fdump.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/fdump.arf

$(BIN_DIR)/fdump.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/fdump/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/fdump.arf
	echo "src/fdump/ffdump.rel" >> $(BIN_DIR)/fdump.arf
	echo "src/fdump/fdump.rel" >> $(BIN_DIR)/fdump.arf
	sed -i '/^$$/d' $(BIN_DIR)/fdump.arf

$(FDUMP_DIR)/fdump.rel: $(FDUMP_DIR)/fdump.c
	$(CCC) $(ECCC_FLAGS) -o $(FDUMP_DIR)/fdump.rel $<

$(FDUMP_DIR)/ffdump.rel: $(FDUMP_DIR)/ffdump.c
	$(CCC) $(ECCC_FLAGS) -o $(FDUMP_DIR)/ffdump.rel $<
