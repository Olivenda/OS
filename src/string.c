#include "os.h"
int strcmp(const char*a,const char*b){ while(*a && (*a==*b)){ a++; b++; } return *(const unsigned char*)a - *(const unsigned char*)b; }
int strncmp(const char*a,const char*b,size_t n){ while(n && *a && (*a==*b)){ a++; b++; n--; } if(n==0) return 0; return *(const unsigned char*)a - *(const unsigned char*)b; }
size_t strlen(const char*s){ size_t i=0; while(s[i]) i++; return i; }
