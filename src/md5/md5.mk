example-md5: $(BIN_DIR)/md5.arf $(BIN_DIR)/md5.com

clean-md5:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/md5.com:	tools $(MD5_OBJS) $(BIN_DIR)/md5.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/md5

MD5_DIR = src/md5
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
MD5_OBJS = $(MD5_DIR)/md5.rel

MD5_ARF = $(BIN_DIR)/md5.arf

$(BIN_DIR)/md5.ihx:	$(MD5_OBJS)
	@echo "Linking md5.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/md5.arf


$(BIN_DIR)/md5.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/md5/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/md5.arf
	echo "src/md5/md5.rel" >> $(BIN_DIR)/md5.arf
	sed -i '/^$$/d' $(BIN_DIR)/md5.arf

$(MD5_DIR)/md5.rel: $(MD5_DIR)/md5.c
	$(CCC) $(ECCC_FLAGS) -o $(MD5_DIR)/md5.rel $<

