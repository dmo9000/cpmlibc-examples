$(BIN_DIR)/bios.bin: $(BIN_DIR)/bios.rel tools
	@echo "Linking $(BIN_DIR)/bios.bin ..."
	sdcc -mz80 --code-loc 0x0 --data-loc 0x90 --no-std-crt0 --nostdlib --nostdinc -o $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.rel
	#sdcc-makebin -p -s 256 $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.bin
	objcopy -Iihex -Obinary $(BIN_DIR)/bios.ihx $(BIN_DIR)/bios.bin
	#hexdump -C $(BIN_DIR)/bios.bin
	#od -A x -t x1z -v $(BIN_DIR)/bios.bin

