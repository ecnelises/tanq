#include "libc/string.h"
#include "display.h"
#include "keyboard.h"
#include "shell.h"
#include "utils.h"
#include "memory.h"
#include "filesystem.h"
#include "interrupt.h"

struct screen scr;
struct terminal term;
struct terminal *curterm;

#define char_visible(c) ((c) >= 32 && (c) <= 127)

/*
 * 初始化必要的控制器
 */
void init_kernel(void)
{
    /* 初始化内存管理模块 */
    mm_init();
    /* 初始化 PS/2 键盘控制器 */
    *(volatile unsigned *)(0x10006000) = 0x00000013;
    /* 启用次级中断控制器 */
    *(volatile unsigned *)(0x10003008) = 0x08;
    /* 初始化屏幕显示 */
    term_full(&scr, &term);
    curterm = &term;
    TIMER2_DATA = 1000;
    TIMER2_CTRL = 0xE2; /* 周期性，32 位计时器 */
    PIC_ENSET = 0x30;
}

void init_welcome(void)
{
    screen_init(&scr, SCR_SVGA);
    struct screen_point pt;
    for (unsigned i = 0; i < scr.width; ++i) {
        pt.x = i;
        for (unsigned j = 0; j < scr.height; ++j) {
            pt.y = j;
            screen_draw_point(&scr, pt, COLOR_RGB(0x13, 0x5C, 0x9A));
        }
    }
    unsigned lm = scr.width / 3;
    unsigned rm = scr.width / 3 * 2;
    unsigned top = scr.height / 3 * 2;
    unsigned bottom = top + 20;
    for (unsigned i = lm; i < rm; ++i) {
        pt.x = i;
        for (unsigned j = top; j < bottom; ++j) {
            pt.y = j;
            screen_draw_point(&scr, pt, COLOR_WHITE);
        }
        sleep(10);
    }
}

int main(void)
{
    // init_welcome();
    screen_init(&scr, SCR_SVGA);
    init_kernel();
    init_fs();
    init_shell();
    unsigned key;
    for (;;) {
        /* 如果发生键盘中断 */
        if (SEC_INTERRUPT_STATUS & 0x08) {
            /* 等待数据传输 */
            while (KMI_STATUS & 0x8);
            /* 传输完成 */
            if (KMI_STATUS & 0x10) {
                key = KMI_DATA & 0xFF;
                handle_key(key, global_key_handler);
                /* 暂时屏蔽然后启用中断 */
                SIC_ENCLR = 0x08;
                SIC_ENSET = 0x08;
            }
        }
        if (PIC_INTERRUPT_STATUS & 0x20) {
            ++ms_since_init;
            TIMER2_CLR = 1;
        }
    }

	return 0;
}
