
#include "app/terminal.h"
#include "common/log.h"
#include "fs/fs.h"

static int pwd_cmd_handler(list_ifc *args) {
    if (args->size(args) != 1) {
        return -1;
    }

    printf("%s\n", fs_current_path_get());

    return 0;
}


TERMINAL_CMD(pwd, pwd_cmd_handler);
