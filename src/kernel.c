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
size_t strlen(const char* s){size_t i=0;while(s[i])i++;return i;}

void clear_screen(){
    for(size_t i=0;i<80*25;i++)video[i]=((uint16_t)0x07<<8)|' ';
    cursor=0;
}

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

struct file {
    char name[16];
    char data[512];
};

static struct file files[16];
static size_t file_count = 0;

static const char file_readme[] = "This is a simple file.\n";

static void init_filesystem() {
    for (size_t i = 0; i < 16; i++) {
        files[i].name[0] = 0;
        files[i].data[0] = 0;
    }
    files[0].name[0] = 'R';
    files[0].name[1] = 'E';
    files[0].name[2] = 'A';
    files[0].name[3] = 'D';
    files[0].name[4] = 'M';
    files[0].name[5] = 'E';
    files[0].name[6] = 0;
    size_t i=0; while (file_readme[i]) { files[0].data[i] = file_readme[i]; i++; }
    files[0].data[i] = 0;
    file_count = 1;
}

static int find_file(const char* name) {
    for (size_t i = 0; i < file_count; i++) {
        size_t j = 0;
        while (files[i].name[j] && name[j] && files[i].name[j] == name[j]) j++;
        if (files[i].name[j] == 0 && name[j] == 0) return (int)i;
    }
    return -1;
}

void shell() {
    char buf[80];
    while (1) {
        print("$ ");
        input(buf, sizeof(buf));
        if (!buf[0]) continue;

        char* cmd = buf;
        char* arg = buf;
        while (*arg && *arg != ' ') arg++;
        if (*arg) { *arg++ = 0; while (*arg == ' ') arg++; }
        else arg = NULL;

        if (!strcmp(cmd, "help")) {
            print("Commands: help ls cat echo clear halt reboot mkdir nano\n");
        } else if (!strcmp(cmd, "ls")) {
            for (size_t i=0;i<file_count;i++) { print(files[i].name); print("\n"); }
        } else if (!strcmp(cmd, "cat")) {
            if (!arg || !*arg) { print("Usage: cat <name>\n"); }
            else {
                const char* name = arg;
                int idx = find_file(name);
                if (idx >= 0) print(files[idx].data); else print("File not found\n");
            }
        } else if (!strcmp(cmd, "echo")) {
            if (arg) print(arg);
            print("\n");
        } else if (!strcmp(cmd, "mkdir")) {
            if (!arg || !*arg) { print("Usage: mkdir <name>\n"); }
            else { print("Directory '"); print(arg); print("' created\n"); }
        } else if (!strcmp(cmd, "nano")) {
            if (!arg || !*arg) { print("Usage: nano <name>\n"); }
            else {
                int idx = find_file(arg);
                if (idx < 0 && file_count < 16) {
                    idx = (int)file_count++;
                    size_t j = 0; while(arg[j] && j < 15) { files[idx].name[j] = arg[j]; j++; }
                    files[idx].name[j] = 0;
                    files[idx].data[0] = 0;
                }
                if (idx >= 0) {
                    print("Enter text, empty line to finish:\n");
                    char line[80];
                    size_t pos = 0;
                    while (1) {
                        input(line, sizeof(line));
                        if (!line[0]) break;
                        size_t l = strlen(line);
                        if (pos + l + 1 >= sizeof(files[idx].data)) break;
                        for (size_t k=0;k<l;k++) files[idx].data[pos++] = line[k];
                        files[idx].data[pos++] = '\n';
                    }
                    files[idx].data[pos] = 0;
                } else {
                    print("No space for new file\n");
                }
            }
        } else if (!strcmp(cmd, "clear")) {
            clear_screen();
        } else if (!strcmp(cmd, "halt")) {
            print("Halting...\n");
            for(;;) __asm__("hlt");
        } else if (!strcmp(cmd, "reboot")) {
            outb(0x64, 0xFE);
            for(;;) __asm__("hlt");
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

__attribute__((section(".text.entry")))
void kernel_entry() {
    init_filesystem();
    kernel_main();
}
