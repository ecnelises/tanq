#ifndef MEMORY_H
#define MEMORY_H

#define PAGENUM sizeof(unsigned) * 256

/*
 * 由于我们在这里并没有开启 MMU，所以还无法做到大连续块的内存分配
 * 这里使用一种简单的将栈和位图结合起来的方式
 * 每 32 个页（每页 4 KB）构成一个组，用位图来表示分配情况
 * 每次分配页的时候，都需要先从栈顶位图找出合适的元素
 * 由于没有虚拟内存系统，单次分配内存最大只能达到 32 页，即 128 KB
 */
struct page_group {
    void *base;
    unsigned bitmap;
    int next;
};

struct page_group_entry {
    void *base;
    int start;
    int end;
};

/* 初始化内存分配栈 */
void mm_init(void);
void *alloc_page(void);

/* n 不能大于 32 */
struct page_group_entry alloc_multipage(unsigned n);
void free_page_entry(struct page_group_entry ptr);

/* 释放单个页面 */
void free_page(void *ptr);

/* 分配内存块，同样不能大于 128 KB，即 131072 */
void *kalloc(unsigned size);
void kfree(void *ptr);

#endif /* MEMORY_H */
