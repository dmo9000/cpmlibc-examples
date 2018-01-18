example-lifegame: $(BIN_DIR)/lifegame.arf $(BIN_DIR)/lifegame.com

clean-lifegame:
	rm -f *.asm *.rel *.lst *.sym

$(BIN_DIR)/lifegame.com:	tools $(LIFEGAME_OBJS) $(BIN_DIR)/lifegame.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/lifegame

LIFEGAME_DIR = src/lifegame
LIBC_DIR = libc
#CFLAGS = --code-loc=0x100 --data-loc=32768
LIFEGAME_OBJS = $(LIFEGAME_DIR)/lifegame.rel

LIFEGAME_ARF = $(BIN_DIR)/lifegame.arf

$(BIN_DIR)/lifegame.ihx:	$(LIFEGAME_OBJS)
	@echo "Linking lifegame.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/lifegame.arf


$(BIN_DIR)/lifegame.arf:	$(LIBC_DIR)/link-static.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/lifegame/' $(LIBC_DIR)/link-static.arf > $(BIN_DIR)/lifegame.arf
	echo "src/lifegame/lifegame.rel" >> $(BIN_DIR)/lifegame.arf
	sed -i '/^$$/d' $(BIN_DIR)/lifegame.arf

$(LIFEGAME_DIR)/lifegame.rel: $(LIFEGAME_DIR)/lifegame.c
	$(CCC) $(ECCC_FLAGS) -o $(LIFEGAME_DIR)/lifegame.rel $<

