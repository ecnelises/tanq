#include "display.h"

/*
 * Color LCD 显示设备相关常量
 * 下面是不同的分辨率对应的控制设置
 * 可以在 ARM 的文档中找到这些数以及对应位的含义
 */

/* QVGA (240x320) 竖屏 */
#define QVGAP_OSCCLK4 0x2C77
#define QVGAP_TIM0    0xC7A7BF38
#define QVGAP_TIM1    0x595B613F
#define QVGAP_TIM2    0x04EF1800

/* QVGA (320x240) 横屏 */
#define QVGAL_OSCCLK4 0x2C77
#define QVGAL_TIM0    0x9F7FBF4C
#define QVGAL_TIM1    0x818360EF
#define QVGAL_TIM2    0x053F1800

/* QCIF (176x220) 横屏 */
#define QCIFP_OSCCLK4 0x2C77
#define QCIFP_TIM0    0xE7C7BF28
#define QCIFP_TIM1    0x8B8D60DB
#define QCIFP_TIM2    0x04AF1800

/* VGA (640x480) */
#define VGA_OSCCLK4   0x2C77
#define VGA_TIM0      0x3F1F3F9C
#define VGA_TIM1      0x090B61DF
#define VGA_TIM2      0x067F1800

/* SVGA (800x600) */
#define SVGA_OSCCLK4  0x2CAC
#define SVGA_TIM0     0x1313A4C4
#define SVGA_TIM1     0x0505F657
#define SVGA_TIM2     0x071F1800

/* Epson QCIF (176x220) */
#define EQCIF_OSCCLK4 0x2C2A
#define EQCIF_TIM0    0x00000128
#define EQCIF_TIM1    0x000000DB
#define EQCIF_TIM2    0x04AF1800

/* Sanyo QVGA (320x240) */
#define SQVGA_OSCCLK4 0x2C2A
#define SQVGA_TIM0    0x0505054C
#define SQVGA_TIM1    0x050514EF
#define SQVGA_TIM2    0x053F1800

static unsigned constants[7][6] = {
    { 240, 320, QVGAP_OSCCLK4, QVGAP_TIM0, QVGAP_TIM1, QVGAP_TIM2 },
    { 320, 240, QVGAL_OSCCLK4, QVGAL_TIM0, QVGAL_TIM1, QVGAL_TIM2 },
    { 176, 220, QCIFP_OSCCLK4, QCIFP_TIM0, QCIFP_TIM1, QCIFP_TIM2 },
    { 640, 480, VGA_OSCCLK4, VGA_TIM0, VGA_TIM1, VGA_TIM2 },
    { 800, 600, SVGA_OSCCLK4, SVGA_TIM0, SVGA_TIM1, SVGA_TIM2 },
    { 176, 200, EQCIF_OSCCLK4, EQCIF_TIM0, EQCIF_TIM1, EQCIF_TIM2 },
    { 320, 240, SQVGA_OSCCLK4, SQVGA_TIM0, SQVGA_TIM1, SQVGA_TIM2 },
};

void screen_init(struct screen * scr, int type)
{
    if (type < 0x00 || type > 0x06) {
        return;
    }

    /* 显存基址固定在 0x00100000 */
    scr->base = (volatile unsigned *)0x00100000;
    *(volatile unsigned *)(0x10120010) = 0x00100000;
    scr->width = constants[type][0];
    scr->height = constants[type][1];

    /* 显示参数设置 */
    *(volatile unsigned *)(0x1000001C) = constants[type][2];
    *(volatile unsigned *)(0x10120000) = constants[type][3];
    *(volatile unsigned *)(0x10120004) = constants[type][4];
    *(volatile unsigned *)(0x10120008) = constants[type][5];

    /* LCDControl 寄存器 */
    *(volatile unsigned *)(0x10120018) = 0x82B;
}
