#include "kernel/kernel.h"
#include "kernel/init.h"

void kernel(void) {
    init_subsystems();

    while(1);
}
