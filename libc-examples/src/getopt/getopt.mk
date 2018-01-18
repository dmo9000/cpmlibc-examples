example-getopt: $(BIN_DIR)/getopt.arf $(BIN_DIR)/getopt.com

clean-getopt:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/getopt.com:	tools $(GETOPT_OBJS) $(BIN_DIR)/getopt.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/getopt

GETOPT_DIR = src/getopt
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
GETOPT_OBJS = $(GETOPT_DIR)/getopt.rel

GETOPT_ARF = $(BIN_DIR)/getopt.arf

$(BIN_DIR)/getopt.ihx:	$(GETOPT_OBJS)
	@echo "Linking getopt.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/getopt.arf


$(BIN_DIR)/getopt.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/getopt/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/getopt.arf
	echo "src/getopt/getopt.rel" >> $(BIN_DIR)/getopt.arf
	sed -i '/^$$/d' $(BIN_DIR)/getopt.arf

$(GETOPT_DIR)/getopt.rel: $(GETOPT_DIR)/getopt.c
	$(CCC) $(ECCC_FLAGS) -o $(GETOPT_DIR)/getopt.rel $<

