example-cvi: $(BIN_DIR)/cvi.arf $(BIN_DIR)/cvi.com

clean-cvi:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/cvi.com:	tools $(CVI_OBJS) $(BIN_DIR)/cvi.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/cvi
	cp $(BIN_DIR)/cvi.com $(BIN_DIR)/vi.com

CVI_DIR = src/cvi
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
CVI_OBJS = $(CVI_DIR)/cvi.rel

CVI_ARF = $(BIN_DIR)/cvi.arf

$(BIN_DIR)/cvi.ihx:	$(CVI_OBJS)
	@echo "Linking cvi.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/cvi.arf


$(BIN_DIR)/cvi.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/cvi/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/cvi.arf
	echo "src/cvi/cvi.rel" >> $(BIN_DIR)/cvi.arf
	sed -i '/^$$/d' $(BIN_DIR)/cvi.arf

$(CVI_DIR)/cvi.rel: $(CVI_DIR)/cvi.c
	$(CCC) $(ECCC_FLAGS) -o $(CVI_DIR)/cvi.rel $<

