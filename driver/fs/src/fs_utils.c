#include "fs/fs.h"
#include "lib/string.h"
#include "lib/parser.h"
#include "kernel/syscall.h"

#define DEFAULT_PATH "/"

const char *curr_path = NULL;

const char *fs_current_path_get(void) {
    if (!curr_path) {
        curr_path = strdup(DEFAULT_PATH);
        ASSERT(curr_path);
    }

    return curr_path;
}

void fs_current_path_set(const char *path) {
    if (curr_path) {
        free((void *) curr_path);
    }
    curr_path = strdup(path);
    ASSERT(curr_path);
}

void normalize_path(char *path) {
    //! TODO: do it without the whole oil of the universe
    if (path && *path) {
        list_ifc *entries = parse_buffer(path, '/', 0);
        if (entries) {
            strcpy(path, "/");

            const list_node *entry = entries->get_front(entries);
            while (entry) {
                const char *str = entry->ptr;

                if (!memcmp(str, ".", 2)) {
                    const list_node *tmp = entry;
                    entry = entry->nxt;
                    entries->delete(entries, tmp, free);
                    continue;
                }

                if (!memcmp(str, "..", 3)) {
                    const list_node *tmp = entry;
                    const list_node *prv = entry->prv;
                    entry = entry->nxt;
                    entries->delete(entries, tmp, free);
                    if (prv) {
                        entries->delete(entries, prv, free);
                    }
                    continue;
                }

                entry = entry->nxt;
            }

            entry = entries->get_front(entries);
            while (entry) {
                *path++ = '/';

                strcpy(path, entry->ptr);

                path += strlen(entry->ptr);

                entry = entry->nxt;
            }

            entries->destroy(entries, free);
        }
    }
}