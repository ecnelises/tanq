#include "shell.h"
#include "display.h"
#include "keyboard.h"
#include "utils.h"
#include "libc/string.h"
#include "filesystem.h"

#define HELP_TEXT \
    "New to this shell?\n" \
    "This is tanq, a simple OS running on ARM versatile platform.\n" \
    "Type 'cmd' for a list of available commands.\n"

char term_buf[64];
unsigned term_buf_len;

/* 初始化控制台 */
void init_shell(void)
{
    print("shell >");
    term_buf_len = 0;
    term_buf[0] = '\0';
}

static unsigned parse_color(const char *str)
{
    if (!strcmp(str, "white")) { return COLOR_RGB(255, 255, 255); }
    if (!strcmp(str, "black")) { return COLOR_RGB(0, 0, 0); }
    if (!strcmp(str, "green")) { return COLOR_RGB(0, 255, 0); }
    if (!strcmp(str, "blue")) { return COLOR_RGB(0, 0, 255); }
    if (!strcmp(str, "red")) { return COLOR_RGB(255, 0, 0); }
    if (!strcmp(str, "purple")) { return COLOR_RGB(128, 0, 128); }
    if (!strcmp(str, "yellow")) { return COLOR_RGB(255, 255, 0); }
    if (!strcmp(str, "orange")) { return COLOR_RGB(255, 165, 0); }
    if (!strcmp(str, "aqua")) { return COLOR_RGB(0, 255, 255); }
    unsigned vals[6];
    for (unsigned i = 0; i < 6; ++i) {
        if (str[i] <= 'F' && str[i] >= 'A') {
            vals[i] = str[i] - 'A' + 10;
        } else if (str[i] <= 'f' && str[i] >= 'a') {
            vals[i] = str[i] - 'a' + 10;
        } else if (str[i] <= '9' && str[i] >= '0') {
            vals[i] = str[i] - '0';
        } else {
            vals[i] = 0xF;
        }
    }
    return (vals[0] << 4) | (vals[1]) | (vals[2] << 12) |
        (vals[3] << 8) | (vals[4] << 20) | (vals[5] << 16);
}

void execute(char *command)
{
    term_putchar(curterm, '\n');
    if (!strcmp(command, "ls")) {
        for (unsigned i = 0; i < FILEMAX; ++i) {
            if (fs[i].name[0] != '\0') {
                print(fs[i].name);
                print("\n");
            }
        }
    } else if (!strcmp(command, "help")) {
        print(HELP_TEXT);
    } else if (!strncmp(command, "chbg", 4)) {
        chbg(curterm, parse_color(command + 5));
    } else if (!strncmp(command, "chfg", 4)) {
        chfg(curterm, parse_color(command + 5));
    } else if (!strcmp(command, "clear")) {
        term_clear(curterm);
    } else if (!strcmp(command, "time")) {
        char buf[10];
        itoa(ms_since_init / 1000, buf);
        print(buf);
        print(".");
        unsigned fraction = ms_since_init % 1000, modified = 0;
        if (fraction < 100) {
            fraction += 100;
            modified = 1;
        }
        itoa(fraction, buf);
        if (modified) {
            buf[0] = '0';
        }
        print(buf);
        print(" seconds elapsed since kernel init.\n");
    } else if (command[0] == '\0') {
    } else if (!strcmp(command, "splitv")) {
        term_clear(curterm);
        curterm->width /= 2;
    } else if (!strcmp(command, "splits")) {
        term_clear(curterm);
        curterm->height /= 2;
    }else {
        print(command);
        print(": command not found.\n");
    }
}

void shell_key_handler(unsigned k)
{
    /* 获取按键的键位值 */
    char realchar = k & 0xFF;
    /* 判断 SHIFT 键 */
    if (k & LSHIFT_MASK || k & RSHIFT_MASK) {
        if (realchar >= 'a' && realchar <= 'z') {
            realchar += ('A' - 'a');
        } else {
            switch (realchar) {
            case '`': realchar = '~'; break;
            case '1': realchar = '!'; break;
            case '2': realchar = '@'; break;
            case '3': realchar = '#'; break;
            case '4': realchar = '$'; break;
            case '5': realchar = '%'; break;
            case '6': realchar = '^'; break;
            case '7': realchar = '&'; break;
            case '8': realchar = '*'; break;
            case '9': realchar = '('; break;
            case '0': realchar = ')'; break;
            case '-': realchar = '_'; break;
            case '=': realchar = '+'; break;
            case '[': realchar = '{'; break;
            case ']': realchar = '}'; break;
            case ';': realchar = ';'; break;
            case '\'': realchar = '\"'; break;
            case ',': realchar = '<'; break;
            case '.': realchar = '>'; break;
            case '/': realchar = '?'; break;
            case '\\': realchar = '|'; break;
            default: break;
            }
        }
    }
    if (term_buf_len < 63) {
        if (realchar == '\n') {
            term_buf[term_buf_len++] = '\0';
            execute(term_buf);
            term_buf_len = 0;
            print("shell >");
        } else if (realchar == '\b') {
            if (term_buf_len > 0) {
                term_unput(curterm);
                --term_buf_len;
            }
        } else if (realchar == '\t') {
        } else {
            term_putchar(curterm, realchar);
            term_buf[term_buf_len++] = realchar;
        }
    }
}
