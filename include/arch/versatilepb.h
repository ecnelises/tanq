/*
 * versatilepb.h
 *
 * 此文件包含 RealView Platform Baseboard (ARM926EJ-S) 平台相关的常量定义
 * 在 QEMU 模拟器里，该平台被称为 versatilepb
 */

#ifndef ARCH_VERSATILEPB_H
#define ARCH_VERSATILEPB_H

/*
 * UART 相关常量，作为串口的统一接口
 * 在一般的模拟器上，串口数据会被当作调试信息写入控制台
 */

/* 四个 UART 寄存器的地址 */
#define UART0 ((volatile unsigned int*)0x101F1000)
#define UART1 ((volatile unsigned int*)0x101F2000)
#define UART2 ((volatile unsigned int*)0x101F3000)
#define UART3 ((volatile unsigned int*)0x10009000)

/* UART 相关寄存器的访问偏移量 */
/* 注意系统是 32 位，所以这里的偏移量都是除以 4 之后的结果 */
#define UARTFR        0x06 /* 状态标志寄存器 */
#define UARTIMSC      0x0E /* 中断状态寄存器 */
#define UARTICR       0x11 /* 中断清除寄存器 */ 

/* 寄存器相关的内容，TODO 准备做成宏或者结构体 */
#define UARTFR_RXFE   0x10
#define UARTFR_TXFF   0x20
#define UARTIMSC_RXIM 0x10
#define UARTIMSC_TXIM 0x20
#define UARTICR_RXIC  0x10
#define UARTICR_TXIC  0x20

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

#define LCD_CTRL   0x10120018 /* LCD 控制器 */
#define LCD_CTRL_D 0x0000082B /* TODO 一个能用的设置，要细化 */

/* TODO 颜色现在的格式是 0x00BBGGRR，要看一下怎么查询格式 */

/* 预设显示设置 */
#define DISP_QVGAP 0x01
#define DISP_QVGAL 0x02
#define DISP_QCIFP 0x03
#define DISP_VGA   0x04
#define DISP_SVGA  0x05
#define DISP_EQCIF 0x06
#define DISP_SQVGA 0x07

/* 分辨率 */
struct disp_resolution {
    unsigned int oscclk4;
    unsigned int clcd_tim0;
    unsigned int clcd_tim1;
    unsigned int clcd_tim2;
};

/* 用于屏幕显示的结构 */
struct disp_screen {
    struct disp_resolution resolution; /* 分辨率 */
    volatile unsigned int *frame_base; /* 显存基址 */
    unsigned int control;              /* 控制位 */
    unsigned int line_npixel;          /* 行长度 */
    unsigned int column_npixel;        /* 列长度 */
};

/* 像素点，从左上到右下 */
struct disp_point {
    unsigned int x : 16;
    unsigned int y : 16;
};

/* 颜色 */
struct disp_color {
    unsigned int red : 8;
    unsigned int green : 8;
    unsigned int blue : 8;
    unsigned int padding : 8;
};

/* 初始化显示设置 */
int disp_init(struct disp_screen *screen, int rsl_type, unsigned int base);

/* 绘制像素点 */
int disp_draw_point(struct disp_screen *screen, struct disp_point pos, struct disp_color color);

#endif /* ARCH_VERSATILEPB_H */
