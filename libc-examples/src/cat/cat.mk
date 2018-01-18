example-cat: $(BIN_DIR)/cat.arf $(BIN_DIR)/cat.com

$(BIN_DIR)/cat.com:	tools $(CAT_OBJS) $(BIN_DIR)/cat.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/cat

CAT_DIR = src/cat
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
CAT_OBJS = $(CAT_DIR)/fcat.rel $(CAT_DIR)/cat.rel

CAT_ARF = $(BIN_DIR)/cat.arf

$(BIN_DIR)/cat.ihx:	$(CAT_OBJS)
	@echo "Linking cat.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/cat.arf


$(BIN_DIR)/cat.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/cat/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/cat.arf
	echo "src/cat/fcat.rel" >> $(BIN_DIR)/cat.arf
	echo "src/cat/cat.rel" >> $(BIN_DIR)/cat.arf
	sed -i '/^$$/d' $(BIN_DIR)/cat.arf

$(CAT_DIR)/cat.rel: $(CAT_DIR)/cat.c
	$(CCC) $(ECCC_FLAGS) -o $(CAT_DIR)/cat.rel $<

$(CAT_DIR)/fcat.rel: $(CAT_DIR)/fcat.c
	$(CCC) $(ECCC_FLAGS) -o $(CAT_DIR)/fcat.rel $<
