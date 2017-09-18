#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>

#define FILEMAX 32

struct file {
    char name[16];
    char *content;
    unsigned len;
};

extern struct file fs[FILEMAX];
extern unsigned file_count;

void init_fs(void);
void create_file(const char *name);
void clear_file(const char *name);
char *read_file(const char *name);
void write_file(const char *name, const char *buf);
void delete_file(const char *name);
bool find_file(const char *name);

#endif /* FILESYSTEM_H */
