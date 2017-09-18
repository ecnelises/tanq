#include "memory.h"

#define START_ADDRESS 0x1000000
#define STACK_DEPTH PAGENUM / sizeof(unsigned)
#define GROUP_LEN 128 * 1024
#define STACK_TOP mm_stack[mm_stack_top]

struct page_group mm_stack[STACK_DEPTH];
unsigned mm_stack_top;

struct page_group mm_used_stack[STACK_DEPTH];
unsigned mm_used_top;

void mm_init()
{
    mm_stack_top = 0;
    mm_used_top = STACK_DEPTH;
    for (unsigned i = 0; i < STACK_DEPTH; ++i) {
        mm_stack[i].base = START_ADDRESS + (void*)(i * GROUP_LEN);
        mm_stack[i].bitmap = 0;
    }
}

static void push_used()
{
    mm_used_stack[--mm_used_top].base = STACK_TOP.base;
    mm_used_stack[mm_used_top].bitmap = STACK_TOP.bitmap;
    ++mm_stack_top;
}

struct page_group_entry alloc_page(void)
{
    struct page_group_entry entry;
    if (mm_stack_top >= STACK_DEPTH) {
        entry.base = 0;
        entry.start = entry.end = -1;
        return entry;
    }
    for (unsigned i = 0; i < sizeof(unsigned); ++i) {
        if (STACK_TOP.bitmap & (1 << i)) {
            entry.start = entry.end = i;
            entry.base = STACK_TOP.base;
            STACK_TOP.bitmap |= 1 << i;
            break;
        }
    }
    if (STACK_TOP.bitmap == (unsigned)~0) {
        push_used();
    }
    return entry;
}

static void find_upper_entry(struct page_group_entry *entry, unsigned n)
{
    ++mm_stack_top;
    if (mm_stack_top >= STACK_DEPTH) {
        entry->base = 0;
        entry->start = entry->end = -1;
        --mm_stack_top;
        return;
    }
    push_used();
    /* 直接分配后面的 */
    entry->base = STACK_TOP.base;
    entry->end = 31;
    entry->start = 31 - n + 1;
    unsigned k = 1;
    for (unsigned i = 0; i < n; ++i) {
        STACK_TOP.bitmap |= k;
        k <<= 1;
    }
}

struct page_group_entry alloc_multipage(unsigned n)
{
    struct page_group_entry entry;
    if (n > 32 || mm_stack_top >= STACK_DEPTH) {
        entry.base = 0;
        entry.start = entry.end = -1;
        return entry;
    }
    int max_cnt = -1;
    int cur_cnt = -1;
    int max_start = -1;
    int cur_start = -1;
    for (unsigned i = 0; i < sizeof(unsigned); ++i) {
        if (STACK_TOP.bitmap & (1 << i)) {
            if (cur_start == -1) {
                cur_start = i;
                cur_cnt = 0;
            }
            ++cur_cnt;
        } else {
            if (cur_cnt > max_cnt) {
                max_start = cur_start;
                max_cnt = cur_cnt;
            }
            cur_start = cur_cnt = -1;
        }
    }
    if (cur_cnt > max_cnt) {
        max_start = cur_start;
        max_cnt = cur_cnt;
    }
    /* 找到了可以适应的块 */
    if (max_cnt >= n) {
        entry.base = STACK_TOP.base;
        entry.start = max_start;
        entry.end = max_start + max_cnt - 1;
    } else {
        find_upper_entry(&entry, n);
    }
    return entry;
}

void free_page(struct page_group_entry ptr);

/* 分配内存块，同样不能大于 128 KB，即 131072 */
void *kalloc(unsigned size);
void kfree(void *ptr);

