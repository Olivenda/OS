NASM=nasm
CC=gcc
CFLAGS=-m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector
LD=ld
LDFLAGS=-melf_i386 -T src/link.ld

all: os-image

bootloader.bin: src/bootloader.asm
	$(NASM) -f bin $< -o $@

kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: kernel.o src/link.ld
	$(LD) $(LDFLAGS) kernel.o -o kernel.bin

os-image: bootloader.bin kernel.bin
	cat $^ > os-image

run: os-image
	qemu-system-i386 -drive format=raw,file=os-image

clean:
	rm -f *.bin *.o os-image
