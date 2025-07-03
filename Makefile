CC=gcc
CFLAGS_BASE=-m64 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector
CFLAGS_EXTRA=-fno-pie -no-pie
CFLAGS=$(CFLAGS_BASE) $(CFLAGS_EXTRA)
UEFI_CFLAGS=-I/usr/include/efi -I/usr/include/efi/x86_64 -fpic -mno-red-zone -fno-stack-protector -DEFI_FUNCTION_WRAPPER
UEFI_LDFLAGS=-nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L/usr/lib -lefi -lgnuefi /usr/lib/crt0-efi-x86_64.o
LD=ld
LDFLAGS=-melf_x86_64 -T src/link.ld

ISO=os-image.iso
ISO_DIR=iso

all: $(ISO)

bootx64.efi: src/uefi_boot.c
	$(CC) $(UEFI_CFLAGS) -c $< -o uefi_boot.o
	$(LD) $(UEFI_LDFLAGS) uefi_boot.o -o bootx64.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc -j .eh_frame -j .bss -j .rodata --target=efi-app-x86_64 bootx64.so $@

KERNEL_SRC = src/kernel.c src/console.c src/ata.c src/filesystem.c src/shell.c src/string.c
KERNEL_OBJS = $(KERNEL_SRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(KERNEL_OBJS) src/link.ld
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin

$(ISO): kernel.bin bootx64.efi
	mkdir -p $(ISO_DIR)/EFI/BOOT
	cp bootx64.efi $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI
	cp kernel.bin $(ISO_DIR)/kernel.bin
	xorriso -as mkisofs -R \
	-e EFI/BOOT/BOOTX64.EFI -no-emul-boot \
	-isohybrid-gpt-basdat -o $@ $(ISO_DIR)
	rm -rf $(ISO_DIR)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -nographic

clean:
	rm -f *.bin *.o *.so bootx64.efi $(ISO)
	rm -rf $(ISO_DIR)
