#include "os.h"

void kernel_main(){
    print("\nSimple OS\nType 'help' for commands.\n");
    shell();
    for(;;);
}

__attribute__((section(".text.entry")))
void kernel_entry(){
    init_filesystem();
    kernel_main();
}
