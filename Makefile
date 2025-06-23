NASM=nasm
CC=gcc
CFLAGS_BASE=-m64 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector
CFLAGS_EXTRA=-fno-pie -no-pie
CFLAGS=$(CFLAGS_BASE) $(CFLAGS_EXTRA)
LD=ld
LDFLAGS=-melf_x86_64 -T src/link.ld

ISO=os-image.iso
ISO_DIR=iso

all: $(ISO)

bootloader.bin: src/bootloader.asm
	$(NASM) -f bin $< -o $@

kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: kernel.o src/link.ld
	$(LD) $(LDFLAGS) kernel.o -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin

os-image: bootloader.bin kernel.bin
	cat $^ > $@

$(ISO): os-image
	mkdir -p $(ISO_DIR)
	cp $< $(ISO_DIR)/boot.img
	xorriso -as mkisofs -R -b boot.img -no-emul-boot -boot-load-size 4 -boot-info-table -o $@ $(ISO_DIR)
	rm -rf $(ISO_DIR)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -nographic

clean:
	rm -f *.bin *.o os-image $(ISO)
	rm -rf $(ISO_DIR)
