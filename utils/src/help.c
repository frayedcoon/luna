#include "app/terminal.h"
#include "kernel/thread.h"
#include "lib/string.h"
#include "common/log.h"
#include "common/sys.h"
#include "kernel/syscall.h"
#include "kernel/mutex.h"
#include "platform/clock.h"
#include "platform/spi.h"

#include "driver/sd_spi.h"
#include "driver/disk.h"
#include "fs/mbr.h"
#include "fs/ext2.h"

static int help_cmd_handler(list_ifc *args) {
    (void) args;

    return 0;
}

TERMINAL_CMD(help, help_cmd_handler);
