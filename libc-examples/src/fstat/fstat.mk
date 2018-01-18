example-fstat: $(BIN_DIR)/fstat.arf $(BIN_DIR)/fstat.com

clean-fstat:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/fstat.com:	tools $(FSTAT_OBJS) $(BIN_DIR)/fstat.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/fstat

FSTAT_DIR = src/fstat
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
FSTAT_OBJS = $(FSTAT_DIR)/fstat.rel

FSTAT_ARF = $(BIN_DIR)/fstat.arf

$(BIN_DIR)/fstat.ihx:	$(FSTAT_OBJS)
	@echo "Linking fstat.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/fstat.arf


$(BIN_DIR)/fstat.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/fstat/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/fstat.arf
	echo "src/fstat/fstat.rel" >> $(BIN_DIR)/fstat.arf
	sed -i '/^$$/d' $(BIN_DIR)/fstat.arf

$(FSTAT_DIR)/fstat.rel: $(FSTAT_DIR)/fstat.c
	$(CCC) $(ECCC_FLAGS) -o $(FSTAT_DIR)/fstat.rel $<

