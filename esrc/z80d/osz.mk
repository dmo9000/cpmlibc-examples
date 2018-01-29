example-osz: $(BIN_DIR)/osz.com

$(BIN_DIR)/osz.com:	tools $(BIN_DIR)/osz.ihx
	$(LBIN_DIR)/load $(BIN_DIR)/osz

$(BIN_DIR)/osz.ihx:	libraries $(BIN_DIR)/lldetect.rel $(BIN_DIR)/detect.rel $(BIN_DIR)/testllio.rel $(BIN_DIR)/seldsk.rel $(BIN_DIR)/debug.rel $(BIN_DIR)/reset.rel $(BIN_DIR)/cls.rel $(BIN_DIR)/box.rel $(BIN_DIR)/ttystress.rel $(BIN_DIR)/wait.rel $(BIN_DIR)/halt.rel $(BIN_DIR)/modules.rel $(BIN_DIR)/call.rel $(BIN_DIR)/dump.rel $(BIN_DIR)/clkchk.rel $(BIN_DIR)/osz.rel $(BIN_DIR)/bogomips.rel $(BIN_DIR)/osz.arf $(BIN_DIR)/bios.bin 
	@echo "Linking osz.ihx ..."
	$(CLD) $(CLD_FLAGS) -nf $(BIN_DIR)/osz.arf

$(BIN_DIR)/lldetect.rel: osz/lldetect.s
	$(CAS) $(CAS_FLAGS) $(BIN_DIR)/lldetect.rel $(ESRC_DIR)/osz/lldetect.s

	

$(BIN_DIR)/bios.bin: $(BIN_DIR)/bios.rel tools
	#@echo "Linking $(BIN_DIR)/bios.bin ..."
	#sdcc -mz80 --code-loc 0x0 --data-loc 0x90 --no-std-crt0 --nostdlib --nostdinc -o $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.rel
	#sdcc-makebin -p -s 256 $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.bin
	#objcopy -Iihex -Obinary $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.bin
	#hexdump -C $(BIN_DIR)/bios.bin
	#od -A x -t x1z -v $(BIN_DIR)/bios.bin


#
## FIXME: I'm sure we can do better than manually adding modules to the linker script!
#

$(BIN_DIR)/osz.arf:	$(BIN_DIR)/libc.arf
	$(QUIET)$(SED) 's/$(REPLACE_TAG)/osz/' $(BIN_DIR)/libc.arf > $(BIN_DIR)/osz.arf 
	echo "bin//bogomips.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//clkchk.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//dump.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//call.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//halt.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//modules.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//wait.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//ttystress.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//box.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//cls.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//reset.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//debug.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//seldsk.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//testllio.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//detect.rel" >> $(BIN_DIR)/osz.arf
	echo "bin//lldetect.rel" >> $(BIN_DIR)/osz.arf
	sed -i '/^$$/d' $(BIN_DIR)/osz.arf


# Build CP/M-80 Command File Structure files
$(BIN_DIR)/bios.rel: $(ESRC_DIR)/osz/bios.s
	$(CAS) $(CAS_FLAGS) $(BIN_DIR)/bios.rel $(ESRC_DIR)/osz/bios.s


$(BIN_DIR)/%.rel: $(ESRC_DIR)/osz/%.c
	$(CCC) $(CCC_FLAGS) -o $(BIN_DIR) $<
