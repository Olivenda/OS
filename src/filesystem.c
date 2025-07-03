#include "os.h"
#include "ata.h"
#include "io.h"

struct file files[16];
size_t file_count = 0;

static const char file_readme[] = "This is a simple file.\n";

static void load_filesystem(){
    uint8_t buf[512];
    ata_read_sector(1, buf);
    file_count = buf[0];
    if(file_count>16) file_count = 16;
    for(size_t i=0;i<file_count;i++){
        size_t off = 1 + i*18;
        for(int j=0;j<16;j++) files[i].name[j] = buf[off+j];
        uint16_t size = buf[off+16] | ((uint16_t)buf[off+17]<<8);
        ata_read_sector(2+i, (uint8_t*)files[i].data);
        if(size<512) files[i].data[size] = 0; else files[i].data[511]=0;
    }
    for(size_t i=file_count;i<16;i++){ files[i].name[0]=0; files[i].data[0]=0; }
}

static void save_filesystem(){
    uint8_t buf[512];
    buf[0] = (uint8_t)file_count;
    for(size_t i=0;i<file_count;i++){
        size_t off = 1 + i*18;
        for(int j=0;j<16;j++) buf[off+j] = files[i].name[j];
        uint16_t size = (uint16_t)strlen(files[i].data);
        buf[off+16] = (uint8_t)(size & 0xFF);
        buf[off+17] = (uint8_t)(size >> 8);
        uint8_t sec[512];
        for(int j=0;j<512;j++) sec[j]=0;
        for(int j=0;j<512 && files[i].data[j]; j++) sec[j]=files[i].data[j];
        ata_write_sector(2+i, sec);
    }
    ata_write_sector(1, buf);
}

void sync_filesystem(void){ save_filesystem(); }

void init_filesystem(){
    load_filesystem();
    if(file_count==0){
        for(size_t i=0;i<16;i++){ files[i].name[0]=0; files[i].data[0]=0; }
        const char* name="README";
        size_t j=0; while(name[j]){ files[0].name[j]=name[j]; j++; }
        files[0].name[j]=0;
        j=0; while(file_readme[j]){ files[0].data[j]=file_readme[j]; j++; }
        files[0].data[j]=0;
        file_count=1;
        save_filesystem();
    }
}

int find_file(const char* name){
    for(size_t i=0;i<file_count;i++){
        size_t j=0; while(files[i].name[j] && name[j] && files[i].name[j]==name[j]) j++;
        if(files[i].name[j]==0 && name[j]==0) return (int)i;
    }
    return -1;
}
