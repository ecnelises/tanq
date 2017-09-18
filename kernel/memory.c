#include "memory.h"
#include "utils.h"

#define START_ADDRESS 0x1000000
#define STACK_DEPTH PAGENUM / sizeof(unsigned)
#define GROUP_LEN 32 * 4096

struct page_group mm_stack[STACK_DEPTH];
struct page_group *mm_stack_top;

void mm_init()
{
    mm_stack_top = mm_stack;
    for (unsigned i = 0; i < STACK_DEPTH; ++i) {
        mm_stack[i].base = (void *)(START_ADDRESS + i * GROUP_LEN);
        mm_stack[i].bitmap = 0;
        mm_stack[i].next = i + 1;
    }
    /* 最后一个元素的下一个节点为空，用 -1 表示 */
    mm_stack[STACK_DEPTH - 1].next = -1;
}

void *alloc_page(void)
{
    if (mm_stack_top->next == -1) {
        return 0;
    }
    void *result = 0;
    for (unsigned i = 0; i < sizeof(unsigned); ++i) {
        if (mm_stack_top->bitmap & (1 << i)) {
            result = (void *)((unsigned)mm_stack_top->base + i);
            mm_stack_top->bitmap |= 1 << i;
            break;
        }
    }
    if (mm_stack_top->bitmap == (unsigned)~0) {
        mm_stack_top = mm_stack + mm_stack_top->next;
    }
    return result;
}

static void find_upper_entry(struct page_group_entry *entry, unsigned n)
{
    while (mm_stack_top->bitmap != 0) {
        if (mm_stack_top->next == -1) {
            entry->base = 0;
            entry->start = entry->end = -1;
            return;
        }
        mm_stack_top = mm_stack + mm_stack_top->next;
    }
    /* 直接分配后面的 */
    entry->base = mm_stack_top->base;
    entry->end = 31;
    entry->start = 31 - n + 1;
    unsigned k = 1;
    for (unsigned i = 0; i < n; ++i) {
        mm_stack_top->bitmap |= k;
        k <<= 1;
    }
}

struct page_group_entry alloc_multipage(unsigned n)
{
    struct page_group_entry entry;
    if (n > 32 || mm_stack_top->next == -1) {
        entry.base = 0;
        entry.start = entry.end = -1;
        return entry;
    }
    int max_cnt = -1;
    int cur_cnt = -1;
    int max_start = -1;
    int cur_start = -1;
    for (unsigned i = 0; i < sizeof(unsigned); ++i) {
        if (mm_stack_top->bitmap & (1 << i)) {
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
    if (max_cnt >= (int)n) {
        entry.base = mm_stack_top->base;
        entry.start = max_start;
        entry.end = max_start + max_cnt - 1;
    } else {
        find_upper_entry(&entry, n);
    }
    return entry;
}

void free_page_entry(struct page_group_entry ptr)
{
    /* 通过 base 指针的偏移量来计算下标 */
    unsigned df = (unsigned *)ptr.base - (unsigned *)START_ADDRESS;
    unsigned index = df * 4 / GROUP_LEN;
    for (int i = ptr.start; i <= ptr.end; ++i) {
        mm_stack[index].bitmap &= ~(1 << i);
    }
    if (mm_stack[index].bitmap == 0) {
        mm_stack[index].next = mm_stack_top->next;
        mm_stack_top->next = index;
    }
}

void free_page(void *ptr)
{
    unsigned df = (unsigned *)ptr - (unsigned *)START_ADDRESS;
    unsigned index = df * 4 / GROUP_LEN;
    unsigned offset = df * 4 % GROUP_LEN;
    mm_stack[index].bitmap &= ~(1 << offset);
    if (mm_stack[index].bitmap == 0) {
        mm_stack[index].next = mm_stack_top->next;
        mm_stack_top->next = index;
    }
}

/* 分配内存块，同样不能大于 128 KB，即 131072 */
void *kalloc(unsigned size);
void kfree(void *ptr);
