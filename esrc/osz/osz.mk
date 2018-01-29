example-osz: $(BIN_DIR)/osz.arf $(BIN_DIR)/osz.com

$(BIN_DIR)/osz.com:	tools $(BIN_DIR)/osz.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/osz

LIBC_DIR = libc/
CFLAGS = --code-loc=0x100 --data-loc=32768
OSZ_OBJS = $(BIN_DIR)/debug.rel $(BIN_DIR)/fcat.rel $(BIN_DIR)/cat.rel $(BIN_DIR)/shipctl.rel $(BIN_DIR)/tty.rel $(BIN_DIR)/detect.rel $(BIN_DIR)/reset.rel $(BIN_DIR)/cls.rel $(BIN_DIR)/box.rel $(BIN_DIR)/ttystress.rel $(BIN_DIR)/modules.rel $(BIN_DIR)/dump.rel $(BIN_DIR)/clkchk.rel $(BIN_DIR)/osz.rel $(BIN_DIR)/bogomips.rel 

BIOS_OBJS = $(BIN_DIR)/bios.bin
OSZ_ARF = $(BIN_DIR)/osz.arf

$(BIN_DIR)/osz.ihx:	libraries $(OSZ_OBJS)
	@echo "Linking osz.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/osz.arf

#$(BIN_DIR)/lldetect.rel: osz/lldetect.s
#	$(CAS) $(CAS_FLAGS) $(BIN_DIR)/lldetect.rel $(ESRC_DIR)/osz/lldetect.s

#$(BIN_DIR)/llclock.rel: osz/llclock.s
#	$(CAS) $(CAS_FLAGS) $(BIN_DIR)/llclock.rel $(ESRC_DIR)/osz/llclock.s

#
## FIXME: I'm sure we can do better than manually adding modules to the linker script!
#

$(BIN_DIR)/osz.arf:	$(LIBC_DIR)/libc.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/osz/' $(LIBC_DIR)/libc.arf > $(BIN_DIR)/osz.arf 
	echo "bin/bogomips.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/clkchk.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/dump.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/modules.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/ttystress.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/box.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/cls.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/reset.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/detect.rel" >> $(BIN_DIR)/osz.arf
#	echo "bin/lldetect.rel" >> $(BIN_DIR)/osz.arf
#	echo "bin/llclock.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/tty.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/shipctl.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/cat.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/fcat.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/debug.rel" >> $(BIN_DIR)/osz.arf
	echo "bin/osz.rel" >> $(BIN_DIR)/osz.arf
	sed -i '/^$$/d' $(BIN_DIR)/osz.arf


# Build CP/M-80 Command File Structure files
$(BIN_DIR)/bios.rel: $(ESRC_DIR)/osz/bios.s
	$(CAS) $(CAS_FLAGS) $(BIN_DIR)/bios.rel $(ESRC_DIR)/osz/bios.s


$(BIN_DIR)/%.rel: $(ESRC_DIR)/osz/%.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $<
