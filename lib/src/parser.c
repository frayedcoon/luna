#include "lib/parser.h"
#include "lib/syscall.h"

static
char *get_arg(const char *buffer, char *begin,
    char delimiter, int wrapping) {
    static char *static_token = NULL;

    if (!buffer && static_token) {
        return static_token;
    }

    char *end   = begin;

    //! look for entry begin
    while ((*begin == delimiter || (*begin == '\"' && wrapping)) && *begin) {
        begin++;
    }

    //! no more entries left
    if (*begin == '\0') {
        return NULL;
    }

    end = begin;

    //! looking for the end of wrapped entry
    if (*(begin - 1) == '\"' && wrapping) {
        while (*end != '\"' && *end) {
            end++;
        }

        if (*end == 0) {
            return NULL;
        }
    } else {
        while (*end != delimiter && *end) {
            end++;
        }
    }

    end--;

    static_token = end + 1;

    int str_len = end - begin + 1;

    //! allocating and copying entry string
    char *arg = malloc(str_len + 1);
    memcpy(arg, begin, str_len);
    arg[str_len] = 0;

    return arg;
}

list_ifc *parse_buffer(const char* buffer, char delimiter, int wrapping) {
    list_ifc *lst = list_create(malloc, free);
    if (!lst) {
        return NULL;
    }

    char *token = (char*) buffer;

    char *arg = get_arg(buffer, token, delimiter, wrapping);
    while (arg) {
        if (!lst->insert_after(lst, NULL, arg)) {
            lst->destroy(lst, free);
            return NULL;
        }

        token = get_arg(NULL, 0, delimiter, wrapping);
        if (!token) {
            break;
        }

        arg = get_arg(buffer, token, delimiter, wrapping);
    }

    return lst;
}