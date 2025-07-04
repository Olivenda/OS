#include "os.h"
#include "io.h"

static void gui(void);

static void run_command(char* buf){
    char* cmd = buf;
    char* arg = buf;
    while(*arg && *arg!=' ') arg++;
    if(*arg){ *arg++=0; while(*arg==' ') arg++; }
    else arg=NULL;

    if(!strcmp(cmd,"help")){
        print("Commands: help ls cat echo clear halt reboot mkdir nano sync exec gui\n");
    }else if(!strcmp(cmd,"ls")){
        for(size_t i=0;i<file_count;i++){ print(files[i].name); print("\n"); }
    }else if(!strcmp(cmd,"cat")){
        if(!arg||!*arg){ print("Usage: cat <name>\n"); }
        else{ int idx = find_file(arg); if(idx>=0) print(files[idx].data); else print("File not found\n"); }
    }else if(!strcmp(cmd,"echo")){
        if(arg) print(arg); print("\n");
    }else if(!strcmp(cmd,"mkdir")){
        if(!arg||!*arg){ print("Usage: mkdir <name>\n"); }
        else{ print("Directory '"); print(arg); print("' created\n"); }
    }else if(!strcmp(cmd,"nano")){
        if(!arg||!*arg){ print("Usage: nano <name>\n"); }
        else{
            int idx=find_file(arg);
            if(idx<0 && file_count<16){
                idx=(int)file_count++;
                size_t j=0; while(arg[j] && j<15){ files[idx].name[j]=arg[j]; j++; }
                files[idx].name[j]=0; files[idx].data[0]=0;
            }
            if(idx>=0){
                print("Enter text, empty line to finish:\n");
                char line[80]; size_t pos=0;
                while(1){
                    input(line,sizeof(line));
                    if(!line[0]) break;
                    size_t l=strlen(line);
                    if(pos+l+1>=sizeof(files[idx].data)) break;
                    for(size_t k=0;k<l;k++) files[idx].data[pos++]=line[k];
                    files[idx].data[pos++]='\n';
                }
                files[idx].data[pos]=0;
                sync_filesystem();
            }else print("No space for new file\n");
        }
    }else if(!strcmp(cmd,"sync")){
        sync_filesystem();
        print("Synced\n");
    }else if(!strcmp(cmd,"exec")){
        if(!arg||!*arg){ print("Usage: exec <name>\n"); }
        else{ int idx=find_file(arg); if(idx>=0) exec_script(files[idx].data); else print("File not found\n"); }
    }else if(!strcmp(cmd,"gui")){
        gui();
    }else if(!strcmp(cmd,"clear")){
        clear_screen();
    }else if(!strcmp(cmd,"halt")){
        print("Halting...\n"); for(;;) __asm__("hlt");
    }else if(!strcmp(cmd,"reboot")){
        outb(0x64,0xFE); for(;;) __asm__("hlt");
    }else{
        print("Unknown command\n");
    }
}

void exec_script(const char* script){
    char line[80]; size_t pos=0;
    for(size_t i=0;;i++){
        char c=script[i];
        if(c==0 || c=='\n'){
            line[pos]=0; if(pos>0) run_command(line); pos=0; if(c==0) break; }
        else{ if(pos<sizeof(line)-1) line[pos++]=c; }
    }
}

static void gui(void){
    clear_screen();
    print("+--------------------------------------------------+\n");
    print("|                Simple GUI Mode                  |\n");
    print("|  Type commands below or 'exit' to return.       |\n");
    print("+--------------------------------------------------+\n");
    char line[80];
    while(1){
        print("> ");
        input(line, sizeof(line));
        if(!line[0]) continue;
        if(!strcmp(line, "exit")) break;
        run_command(line);
    }
    clear_screen();
}

void shell(void){
    char buf[80];
    while(1){
        print("$ ");
        input(buf,sizeof(buf));
        if(!buf[0]) continue;
        run_command(buf);
    }
}
