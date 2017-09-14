#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define LCTRL_MASK  0x00100
#define LSHIFT_MASK 0x00200
#define LGUI_MASK   0x00400
#define LALT_MASK   0x00800
#define CAPS_MASK   0x01000
#define RSHIFT_MASK 0x02000
#define RGUI_MASK   0x04000
#define RALT_MASK   0x08000
#define RCTRL_MASK  0x10000

void handle_key(uint8_t key, void (*cbk)(unsigned));

#endif /* KEYBOARD_H */
