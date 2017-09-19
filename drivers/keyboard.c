#include <stdint.h>
#include "keyboard.h"

/* Scan Code Set 2 */

#define KBD_KEY_A   0x1C
#define KBD_KEY_B   0x32
#define KBD_KEY_C   0x21
#define KBD_KEY_D   0x23
#define KBD_KEY_E   0x24
#define KBD_KEY_F   0x2B
#define KBD_KEY_G   0x34
#define KBD_KEY_H   0x33
#define KBD_KEY_I   0x43
#define KBD_KEY_J   0x3B
#define KBD_KEY_K   0x42
#define KBD_KEY_L   0x4B
#define KBD_KEY_M   0x3A
#define KBD_KEY_N   0x31
#define KBD_KEY_O   0x44
#define KBD_KEY_P   0x4D
#define KBD_KEY_Q   0x15
#define KBD_KEY_R   0x2D
#define KBD_KEY_S   0x1B
#define KBD_KEY_T   0x2C
#define KBD_KEY_U   0x3C
#define KBD_KEY_V   0x2A
#define KBD_KEY_W   0x1D
#define KBD_KEY_X   0x22
#define KBD_KEY_Y   0x35
#define KBD_KEY_Z   0x1A
#define KBD_KEY_0   0x45
#define KBD_KEY_1   0x16
#define KBD_KEY_2   0x1E
#define KBD_KEY_3   0x26
#define KBD_KEY_4   0x25
#define KBD_KEY_5   0x2E
#define KBD_KEY_6   0x36
#define KBD_KEY_7   0x3D
#define KBD_KEY_8   0x3E
#define KBD_KEY_9   0x46

#define KBD_KEY_BKSP    0x66 /* 退格键 */
#define KBD_KEY_SPACE   0x29 /* 空格键 */
#define KBD_KEY_TAB     0x0D /* 制表符 */
#define KBD_KEY_CAPS    0x58 /* Caps Lock */
/*
 * GUI 键在 Mac 上指 Command 键，PC 上指 Windows 键
 * 但 LGUI 并不能用一个扫描码来表示
 * 前面还需要有一个 0xE0 作为前缀
 * 而按键抬起时候的 Break Code 则是「0xE0 0xF0 扫描码」的顺序
 */
#define KBD_UP_PREFIX   0xF0
#define KBD_SPCL_PREFIX 0xE0
#define KBD_KEY_LGUI_P  0x1F
#define KBD_KEY_RGUI_P  0x27
#define KBD_KEY_LSHIFT  0x12
#define KBD_KEY_RSHIFT  0x59
#define KBD_KEY_LCTRL   0x14
#define KBD_KEY_RCTRL_P 0x14
#define KBD_KEY_LALT    0x11
#define KBD_KEY_RALT_P  0x11
#define KBD_KEY_ENTER   0x5A
#define KBD_KEY_ESC     0x76

#define KBD_KEY_F1      0x05
#define KBD_KEY_F2      0x06
#define KBD_KEY_F3      0x04
#define KBD_KEY_F4      0x0C
#define KBD_KEY_F5      0x03
#define KBD_KEY_F6      0x0B
#define KBD_KEY_F7      0x83
#define KBD_KEY_F8      0x0A
#define KBD_KEY_F9      0x01
#define KBD_KEY_F10     0x09
#define KBD_KEY_F11     0x78
#define KBD_KEY_F12     0x07

#define KBD_KEY_GRAVE   0x0E /* ` */
#define KBD_KEY_MINUS   0x4E /* - */
#define KBD_KEY_EQUAL   0x55 /* = */
#define KBD_KEY_BKSLASH 0X5D /* \ */
#define KBD_KEY_LBRCKET 0x54 /* [ */
#define KBD_KEY_RBRCKET 0x5B /* ] */
#define KBD_KEY_SEMICLN 0x4C /* ; */
#define KBD_KEY_QUOTE   0x52 /* ' */
#define KBD_KEY_COMMA   0x41 /* , */
#define KBD_KEY_DOT     0x49 /* . */
#define KBD_KEY_SLASH   0x4A /* / */

#define KBD_KEY_UP_P    0x75
#define KBD_KEY_DOWN_P  0x72
#define KBD_KEY_LEFT_P  0x6B
#define KBD_KEY_RIGHT_P 0x74

/* 特殊键按下的状态（这些键按住是不会有持续信号的） */
static unsigned special_key_status = 0;
static int spcl_prefix_cnt = 0;
static int up_prefix_cnt = 0;
void (*global_key_handler)(unsigned);

#define SET_BIT(val, mask) ((val) |= (mask))
#define UNSET_BIT(val, mask) ((val) &= ~(mask))

#define SIMPLE_KEY_CHOICE(name, chr) \
    case KBD_KEY_##name : \
    if (up_prefix_cnt) { \
        --up_prefix_cnt; \
    } else { \
        result = chr; \
    } \
    break;

#define UP_SPCL_KEY_CHOICE(name) \
    case KBD_KEY_L##name : \
    if (spcl_prefix_cnt && up_prefix_cnt) { \
        if (special_key_status & R##name##_MASK) { \
            --spcl_prefix_cnt; --up_prefix_cnt; \
            UNSET_BIT(special_key_status, R##name##_MASK); \
        } else { \
            --spcl_prefix_cnt; \
            SET_BIT(special_key_status, R##name##_MASK); \
        } \
    } else if (spcl_prefix_cnt) { \
        --spcl_prefix_cnt; \
        SET_BIT(special_key_status, R##name##_MASK); \
    } else if (up_prefix_cnt) { \
        --up_prefix_cnt; \
        UNSET_BIT(special_key_status, L##name##_MASK); \
    } else { \
        SET_BIT(special_key_status, L##name##_MASK); \
    } \
    break;

#define UP_KEY_CHOICE(name) \
    case KBD_KEY_##name : \
    if (up_prefix_cnt) { \
        --up_prefix_cnt; \
        UNSET_BIT(special_key_status, name##_MASK); \
    } else { \
        SET_BIT(special_key_status, name##_MASK); \
    } \
    break;

#define SPCL_KEY_CHOICE(name) \
    case KBD_KEY_##name##_P : \
    if (spcl_prefix_cnt && up_prefix_cnt) { \
        --up_prefix_cnt; --spcl_prefix_cnt; \
        UNSET_BIT(special_key_status, name##_MASK); \
    } else if (spcl_prefix_cnt) { \
        --spcl_prefix_cnt; \
        SET_BIT(special_key_status, name##_MASK); \
    } \
    break;

void handle_key(uint8_t key, void (*cbk)(unsigned))
{
    unsigned result = 0;
    switch (key) {
    case KBD_SPCL_PREFIX: ++spcl_prefix_cnt; break;
    case KBD_UP_PREFIX: ++up_prefix_cnt; break;
    UP_SPCL_KEY_CHOICE(CTRL)
    UP_SPCL_KEY_CHOICE(ALT)
    UP_KEY_CHOICE(CAPS)
    UP_KEY_CHOICE(LSHIFT)
    UP_KEY_CHOICE(RSHIFT)
    SPCL_KEY_CHOICE(LGUI)
    SPCL_KEY_CHOICE(RGUI)
    SIMPLE_KEY_CHOICE(A, 'a')
    SIMPLE_KEY_CHOICE(B, 'b')
    SIMPLE_KEY_CHOICE(C, 'c')
    SIMPLE_KEY_CHOICE(D, 'd')
    SIMPLE_KEY_CHOICE(E, 'e')
    SIMPLE_KEY_CHOICE(F, 'f')
    SIMPLE_KEY_CHOICE(G, 'g')
    SIMPLE_KEY_CHOICE(H, 'h')
    SIMPLE_KEY_CHOICE(I, 'i')
    SIMPLE_KEY_CHOICE(J, 'j')
    SIMPLE_KEY_CHOICE(K, 'k')
    SIMPLE_KEY_CHOICE(L, 'l')
    SIMPLE_KEY_CHOICE(M, 'm')
    SIMPLE_KEY_CHOICE(N, 'n')
    SIMPLE_KEY_CHOICE(O, 'o')
    SIMPLE_KEY_CHOICE(P, 'p')
    SIMPLE_KEY_CHOICE(Q, 'q')
    SIMPLE_KEY_CHOICE(R, 'r')
    SIMPLE_KEY_CHOICE(S, 's')
    SIMPLE_KEY_CHOICE(T, 't')
    SIMPLE_KEY_CHOICE(U, 'u')
    SIMPLE_KEY_CHOICE(V, 'v')
    SIMPLE_KEY_CHOICE(W, 'w')
    SIMPLE_KEY_CHOICE(X, 'x')
    SIMPLE_KEY_CHOICE(Y, 'y')
    SIMPLE_KEY_CHOICE(Z, 'z')
    SIMPLE_KEY_CHOICE(0, '0')
    SIMPLE_KEY_CHOICE(1, '1')
    SIMPLE_KEY_CHOICE(2, '2')
    SIMPLE_KEY_CHOICE(3, '3')
    SIMPLE_KEY_CHOICE(4, '4')
    SIMPLE_KEY_CHOICE(5, '5')
    SIMPLE_KEY_CHOICE(6, '6')
    SIMPLE_KEY_CHOICE(7, '7')
    SIMPLE_KEY_CHOICE(8, '8')
    SIMPLE_KEY_CHOICE(9, '9')
    SIMPLE_KEY_CHOICE(GRAVE, '`')
    SIMPLE_KEY_CHOICE(MINUS, '-')
    SIMPLE_KEY_CHOICE(EQUAL, '=')
    SIMPLE_KEY_CHOICE(BKSLASH, '\\')
    SIMPLE_KEY_CHOICE(LBRCKET, '[')
    SIMPLE_KEY_CHOICE(RBRCKET, ']')
    SIMPLE_KEY_CHOICE(SEMICLN, ';')
    SIMPLE_KEY_CHOICE(QUOTE, '\'')
    SIMPLE_KEY_CHOICE(COMMA, ',')
    SIMPLE_KEY_CHOICE(DOT, '.')
    SIMPLE_KEY_CHOICE(SLASH, '/')
    SIMPLE_KEY_CHOICE(SPACE, ' ')
    SIMPLE_KEY_CHOICE(ENTER, '\n')
    SIMPLE_KEY_CHOICE(BKSP, '\b')
    default:
        break;
    }
    if (result) {
        result |= special_key_status;
        cbk(result);
    }
}
