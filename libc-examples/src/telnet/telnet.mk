example-telnet: $(BIN_DIR)/telnet.arf $(BIN_DIR)/telnet.com

clean-telnet:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/telnet.com:	tools $(TELNET_OBJS) $(BIN_DIR)/telnet.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/telnet

TELNET_DIR = src/telnet
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768 
TELNET_OBJS = $(TELNET_DIR)/telnet.rel $(TELNET_DIR)/zmodem.rel

TELNET_ARF = $(BIN_DIR)/telnet.arf

$(BIN_DIR)/telnet.ihx:	$(TELNET_OBJS)
	@echo "Linking telnet.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/telnet.arf


$(BIN_DIR)/telnet.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/telnet/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/telnet.arf
	echo "src/telnet/telnet.rel" >> $(BIN_DIR)/telnet.arf
	echo "src/telnet/zmodem.rel" >> $(BIN_DIR)/telnet.arf
	sed -i '/^$$/d' $(BIN_DIR)/telnet.arf


$(TELNET_DIR)/%.rel: $(TELNET_DIR)/%.c
	$(CCC) -D__CPM__ $(ECCC_FLAGS) -o $@ $<
