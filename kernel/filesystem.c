#include "filesystem.h"
#include "memory.h"
#include "utils.h"
#include "libc/string.h"

struct file fs[FILEMAX];
unsigned file_count = 0;

void init_fs(void)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        fs[i].name[0] = '\0';
        fs[i].content = 0;
        fs[i].len = 0;
    }
}

bool create_file(const char *name)
{
    if (file_count == FILEMAX || strlen(name) >= 15) {
        return false;
    }
    ++file_count;
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (fs[i].name[0] == '\0') {
            strncpy(fs[i].name, name, 16);
            break;
        }
    }
    return true;
}

void clear_file(const char *name)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (!strncmp(name, fs[i].name, 16)) {
            if (fs[i].content != 0) {
                free_page(fs[i].content);
                fs[i].content = 0;
            }
            fs[i].len = 0;
            break;
        }
    }
}

void delete_file(const char *name)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (!strncmp(name, fs[i].name, 16)) {
            fs[i].name[0] = '\0';
            if (fs[i].content != 0) {
                free_page(fs[i].content);
                fs[i].content = 0;
            }
            fs[i].len = 0;
            --file_count;
            break;
        }
    }
}

char *read_file(const char *name)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (!strncmp(name, fs[i].name, 16)) {
            return fs[i].content;
        }
    }
    return 0;
}

void write_file(const char *name, const char *buf)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (!strncmp(name, fs[i].name, 16)) {
            if (fs[i].content == 0) {
                fs[i].content = alloc_page();
            }
            strncpy(fs[i].content, buf, 4096);
            fs[i].len = strlen(fs[i].content);
            break;
        }
    }
}

bool find_file(const char *name)
{
    for (unsigned i = 0; i < FILEMAX; ++i) {
        if (!strncmp(name, fs[i].name, 16)) {
            return true;
        }
    }
    return false;
}
