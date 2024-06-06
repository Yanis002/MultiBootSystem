#include "abort_exit.h"
#include "stdio.h"

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
