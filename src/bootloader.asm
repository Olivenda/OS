; 64-bit bootloader that loads the kernel
BITS 16
ORG 0x7c00

start:
    cli
    xor ax, ax
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
    jmp CODE32_SEL:init_pm

[BITS 32]
init_pm:
    mov ax, DATA32_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    call setup_paging

    ; Enable PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ; Load PML4 address
    mov eax, PML4
    mov cr3, eax

    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    jmp CODE64_SEL:long_mode

[BITS 64]
long_mode:
    mov ax, DATA64_SEL
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    mov rax, 0x1000
    jmp rax

hang:
    hlt
    jmp hang

[BITS 16]
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

[BITS 32]
setup_paging:
    ; Set up identity-mapped 2MB page
    mov dword [PD], 0x83
    mov dword [PD+4], 0
    mov eax, PD
    or eax, 0x3
    mov dword [PDPT], eax
    mov dword [PDPT+4], 0
    mov eax, PDPT
    or eax, 0x3
    mov dword [PML4], eax
    mov dword [PML4+4], 0
    ret

boot_msg db 'Booting 64-bit...',0
disk_msg db 'Disk error!',0
BOOT_DRIVE db 0

align 16
GDT:
    dq 0
    dq 0x00CF9A000000FFFF ; 32-bit code
    dq 0x00CF92000000FFFF ; 32-bit data
    dq 0x00AF9A000000FFFF ; 64-bit code
    dq 0x00AF92000000FFFF ; 64-bit data
GDT_END:

CODE32_SEL equ GDT - $$ + 0x08
DATA32_SEL equ GDT - $$ + 0x10
CODE64_SEL equ GDT - $$ + 0x18
DATA64_SEL equ GDT - $$ + 0x20

align 4
gdt_descriptor:
    dw GDT_END - GDT - 1
    dd GDT


PML4 equ 0x2000
PDPT equ 0x3000
PD   equ 0x4000

TIMES 510 - ($-$$) db 0
DW 0xAA55
