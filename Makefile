CC = clang
TARGET = riscv32-unknown-elf
OUTPUT = build/main.elf
SOURCES = main.c memset.c sleepk.c uart.c printk.c logo.c memory.c cd.c file.c ls.c shell.c touch.c
OBJECTS = $(SOURCES:%.c=build/%.o)
LINKER_SCRIPT = kernel.ld
DEPFILES = $(SOURCES:%.c=build/%.d)

HEADERS = main.h types.h memset.h  sleepk.h uart.h printk.h logo.h memory.h cd.h file.h ls.h shell.h touch.h

# Compile flags
CFLAGS = -std=c11 -O2 -g3 -Wall -Wextra -Wconversion -Wpedantic \
         -fno-stack-protector -ffreestanding -nostdlib -static \
         --target=$(TARGET)

# Link flags (inclusief -fuse-ld=lld dat alleen voor linking nodig is)
LDFLAGS = -fuse-ld=lld

# Flags voor dependency generation
DEPFLAGS = -MMD -MP

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS) $(LINKER_SCRIPT) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(OUTPUT) $(OBJECTS)

# Regel om object files te bouwen
build/%.o: %.c $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPFILES)

# Commando om disassembly te bekijken
disasm: $(OUTPUT)
	llvm-objdump $(OUTPUT) -d

# Commando om QEMU te starten
run: $(OUTPUT)
	cd ~/os && \
	qemu-system-riscv32 -m 128M -machine virt -smp 1 \
	  -bios ~/opensbi-1.3-rv-bin/share/opensbi/ilp32/generic/firmware/fw_dynamic.bin \
	  -kernel build/main.elf \
	  -serial mon:stdio -nographic

# Clean alle gegenereerde bestanden
clean:
	rm -rf build

# Clean en rebuild volledig
rebuild: clean all

.PHONY: all clean disasm run rebuild