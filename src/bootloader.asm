; Simple bootloader that loads the kernel
BITS 16
ORG 0x7c00

start:
    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov [BOOT_DRIVE], dl

    mov si, boot_msg
    call print_string

    ; Load 20 sectors from disk to 0x1000
    mov bx, 0x1000
    mov ah, 0x02
    mov al, 20
    mov ch, 0
    mov dh, 0
    mov cl, 2
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

    ; Enable A20 line
    call enable_a20

    ; Enter protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel loaded at 0x1000
    call 0x1000

hang:
    hlt
    jmp hang

print_string:
    pusha
.print_char:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0E
    int 0x10
    jmp .print_char
.done:
    popa
    ret

disk_error:
    mov si, disk_msg
    call print_string
    jmp hang

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

boot_msg db 'Booting...',0
disk_msg db 'Disk error!',0
BOOT_DRIVE db 0

align 4
GDT:
    dq 0
    dw 0xffff, 0x0000, 0x9A00, 0x00CF
    dw 0xffff, 0x0000, 0x9200, 0x00CF
GDT_END:

CODE_SEG equ GDT - GDT + 0x08
DATA_SEG equ GDT - GDT + 0x10

gdt_descriptor:
    dw GDT_END - GDT - 1
    dd GDT

TIMES 510 - ($-$$) db 0
DW 0xAA55
