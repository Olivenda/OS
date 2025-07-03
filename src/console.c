#include "os.h"
#include "io.h"

#define VIDEO_MEMORY (uint16_t*)0xB8000
static uint16_t* const video = VIDEO_MEMORY;
static size_t cursor = 0;

void clear_screen(void){
    for(size_t i=0;i<80*25;i++) video[i]=((uint16_t)0x07<<8)|' ';
    cursor=0;
}

void putchar(char c){
    if(c=='\n') cursor += 80 - (cursor % 80);
    else video[cursor++] = ((uint16_t)0x07<<8)|c;
    if(cursor>=80*25) cursor = 0;
}

void print(const char* s){ while(*s) putchar(*s++); }

void input(char* buf,size_t n){
    size_t i=0;
    while(i<n-1){
        uint8_t sc;
        do{ sc=inb(0x60); }while(sc & 0x80);
        char c=0;
        if(sc==0x1C){ putchar('\n'); break; }
        else if(sc==0x0E){ if(i){ --i; putchar('\b'); putchar(' '); putchar('\b'); } continue; }
        else if(sc>=0x02 && sc<=0x0B) c="1234567890"[sc-0x02];
        else if(sc>=0x10 && sc<=0x19) c="qwertyuiop"[sc-0x10];
        else if(sc>=0x1E && sc<=0x26) c="asdfghjkl"[sc-0x1E];
        else if(sc>=0x2C && sc<=0x32) c="zxcvbnm"[sc-0x2C];
        else if(sc==0x39) c=' ';
        if(c){ buf[i++]=c; putchar(c); }
    }
    buf[i]=0;
}
