#ifndef OS_H
#define OS_H
#include <stddef.h>
#include <stdint.h>

void clear_screen(void);
void putchar(char c);
void print(const char* s);
void input(char* buf, size_t n);

struct file {
    char name[16];
    char data[512];
};

extern struct file files[16];
extern size_t file_count;

void init_filesystem(void);
int find_file(const char* name);
void sync_filesystem(void);

void exec_script(const char* script);
void shell(void);

int strcmp(const char*a,const char*b);
int strncmp(const char*a,const char*b,size_t n);
size_t strlen(const char*s);

#endif
