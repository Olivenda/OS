#include <stddef.h>
#include <stdint.h>

#define VIDEO_MEMORY (uint16_t*)0xB8000
static uint16_t* const video = VIDEO_MEMORY;
static size_t cursor = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
int strcmp(const char *a,const char *b){while(*a&&(*a==*b)){a++;b++;}return *(const unsigned char*)a-*(const unsigned char*)b;}
int strncmp(const char *a,const char *b,size_t n){while(n && *a && (*a==*b)){a++;b++;n--;}if(n==0)return 0;return *(const unsigned char*)a-*(const unsigned char*)b;}

void putchar(char c) {
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
    } else {
        video[cursor++] = ((uint16_t)0x07 << 8) | c;
    }
    if (cursor >= 80*25) cursor = 0;
}

void print(const char* s) {
    while (*s) putchar(*s++);
}

void input(char* buf, size_t n) {
    size_t i = 0;
    while (i < n-1) {
        uint8_t sc;
        do { sc = inb(0x60); } while (sc & 0x80);
        char c = 0;
        if (sc == 0x1C) { // Enter
            putchar('\n');
            break;
        } else if (sc == 0x0E) { // Backspace
            if (i) { --i; putchar('\b'); putchar(' '); putchar('\b'); }
            continue;
        } else if (sc >= 0x02 && sc <= 0x0B) {
            c = "1234567890"[sc-0x02];
        } else if (sc >= 0x10 && sc <= 0x19) {
            c = "qwertyuiop"[sc-0x10];
        } else if (sc >= 0x1E && sc <= 0x26) {
            c = "asdfghjkl"[sc-0x1E];
        } else if (sc >= 0x2C && sc <= 0x32) {
            c = "zxcvbnm"[sc-0x2C];
        } else if (sc == 0x39) {
            c = ' ';
        }
        if (c) { buf[i++] = c; putchar(c); }
    }
    buf[i] = 0;
}

struct file { const char* name; const char* data; };

static const char file_readme[] = "This is a simple file.\n";
static const struct file files[] = {
    {"README", file_readme},
};
static const size_t file_count = sizeof(files)/sizeof(files[0]);

void shell() {
    char buf[80];
    while (1) {
        print("$ ");
        input(buf, sizeof(buf));
        if (!buf[0]) continue;
        if (!strcmp(buf, "help")) {
            print("Commands: help ls cat\n");
        } else if (!strcmp(buf, "ls")) {
            for (size_t i=0;i<file_count;i++) { print(files[i].name); print("\n"); }
        } else if (!strncmp(buf, "cat ", 4)) {
            const char* name = buf+4;
            size_t i;
            for (i=0;i<file_count;i++) if (!strcmp(name, files[i].name)) break;
            if (i<file_count) print(files[i].data); else print("File not found\n");
        } else {
            print("Unknown command\n");
        }
    }
}

void kernel_main() {
    print("\nSimple OS\nType 'help' for commands.\n");
    shell();
    for(;;);
}
