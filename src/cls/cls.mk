example-cls: $(BIN_DIR)/cls.arf $(BIN_DIR)/cls.com

clean-cls:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/cls.com:	tools $(CLS_OBJS) $(BIN_DIR)/cls.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/cls

CLS_DIR = src/cls
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
CLS_OBJS = $(CLS_DIR)/cls.rel

CLS_ARF = $(BIN_DIR)/cls.arf

$(BIN_DIR)/cls.ihx:	$(CLS_OBJS)
	@echo "Linking cls.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/cls.arf


$(BIN_DIR)/cls.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/cls/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/cls.arf
	echo "src/cls/cls.rel" >> $(BIN_DIR)/cls.arf
	sed -i '/^$$/d' $(BIN_DIR)/cls.arf

$(CLS_DIR)/cls.rel: $(CLS_DIR)/cls.c
	$(CCC) $(ECCC_FLAGS) -o $(CLS_DIR)/cls.rel $<

