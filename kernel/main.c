#include "libc/string.h"
#include "display.h"
#include "keyboard.h"
#include "shell.h"
#include "utils.h"
#include "filesystem.h"

struct screen scr;
struct terminal term;
struct terminal *curterm;

#define char_visible(c) ((c) >= 32 && (c) <= 127)

/* 中断状态 */
#define PIC_INTERRUPT_STATUS *(volatile unsigned *)(0x10140000)
#define SEC_INTERRUPT_STATUS *(volatile unsigned *)(0x10003000)

/* 中断控制 */
#define PIC_ENSET *(volatile unsigned *)(0x10140010)
#define PIC_ENCLR *(volatile unsigned *)(0x10140014)
#define SIC_ENSET *(volatile unsigned *)(0x10003008)
#define SIC_ENCLR *(volatile unsigned *)(0x1000300C)
#define TIMER1_DATA *(volatile unsigned *)(0x101E2000)
#define TIMER1_CTRL *(volatile unsigned *)(0x101E2008)
#define TIMER1_CLR  *(volatile unsigned *)(0x101E200C)
#define TIMER3_DATA *(volatile unsigned *)(0x101E3000)
#define TIMER3_CTRL *(volatile unsigned *)(0x101E3008)
#define TIMER3_CLR  *(volatile unsigned *)(0x101E300C)

/*
 * 初始化必要的控制器
 */
void init_kernel(void)
{
    /* 初始化 PS/2 键盘控制器 */
    *(volatile unsigned *)(0x10006000) = 0x00000013;
    /* 启用次级中断控制器 */
    *(volatile unsigned *)(0x10003008) = 0x08;
    /* 初始化屏幕显示 */
    screen_init(&scr, SCR_SVGA);
    term_full(&scr, &term);
    curterm = &term;
    TIMER3_DATA = 1000;
    TIMER3_CTRL = 0xE2; /* 周期性，32 位计时器 */
    PIC_ENSET = 0x30;
}

int main(void)
{
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
                handle_key(key, shell_key_handler);
                /* 暂时屏蔽然后启用中断 */
                SIC_ENCLR = 0x08;
                SIC_ENSET = 0x08;
            }
        }
        if (PIC_INTERRUPT_STATUS & 0x10) {
            bwputs("ha");
        }
        if (PIC_INTERRUPT_STATUS & 0x20) {
            ++ms_since_init;
            TIMER3_CLR = 0;
        }
    }

	return 0;
}
