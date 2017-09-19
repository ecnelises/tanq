#include "shell.h"
#include "display.h"
#include "keyboard.h"
#include "utils.h"
#include "libc/string.h"
#include "memory.h"
#include "filesystem.h"

#define HELP_TEXT \
    "New to this shell?\n" \
    "This is tanq, a simple OS running on ARM versatile platform.\n" \
    "Type 'cmd' for a list of available commands.\n"

char term_buf[64];
char edit_buf[512];
char editing[16];
unsigned edit_buf_len;
unsigned term_buf_len;

void edit_mode(const char *file);

/* 初始化控制台 */
void init_shell(void)
{
    global_key_handler = shell_key_handler;
    print("shell >");
    term_buf_len = 0;
    term_buf[0] = '\0';
}

static void time_elapsed(void)
{
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
            return COLOR_WHITE;
        }
    }
    return (vals[0] << 4) | (vals[1]) | (vals[2] << 12) |
        (vals[3] << 8) | (vals[4] << 20) | (vals[5] << 16);
}

void execute(char *command)
{
    command = next_nonblank(command);
    term_putchar(curterm, '\n');
    if (!strcmp(command, "ls")) {
        for (unsigned i = 0; i < FILEMAX; ++i) {
            if (fs[i].name[0] != '\0') {
                print(fs[i].name);
                print("\n");
            }
        }
        print("----------\n");
        term_printi(curterm, file_count);
        print(" files in total.\n");
    } else if (!strcmp(command, "help")) {
        print(HELP_TEXT);
    } else if (!strncmp(command, "chbg", 4)) {
        chbg(curterm, parse_color(next_nonblank(command + 4)));
    } else if (!strncmp(command, "chfg", 4)) {
        chfg(curterm, parse_color(next_nonblank(command + 4)));
    } else if (!strcmp(command, "clear")) {
        term_clear(curterm);
    } else if (!strcmp(command, "time")) {
        time_elapsed();
    } else if (command[0] == '\0') {
    } else if (!strcmp(command, "splitv")) {
        term_clear(curterm);
        curterm->width /= 2;
    } else if (!strcmp(command, "splits")) {
        term_clear(curterm);
        curterm->height /= 2;
    } else if (!strcmp(command, "sleep")) {
        sleep(3000);
    } else if (!strncmp(command, "create", 6) && command[6] == ' ') {
        const char *filename = next_nonblank(command + 6);
        if (find_file(filename)) {
            print(filename);
            print(" already exists!\n");
        } else if (create_file(filename)) {
            print("Successfully created file.\n");
        } else {
            print("Sorry, unable to create file.\n");
        }
    } else if (!strncmp(command, "cat", 3)) {
        const char *filename = next_nonblank(command + 3);
        if (!find_file(filename)) {
            print("File '");
            print(filename);
            print("' doesn't exist!\n");
        } else {
            print(read_file(filename));
            print("\n");
        }
    } else if (!strncmp(command, "edit", 4) && command[4] == ' ') {
        const char *filename = next_nonblank(command + 4);
        if (!find_file(filename)) {
            print("File '");
            print(filename);
            print("' doesn't exist!\n");
        } else {
            edit_mode(filename);
        }
    } else if (!strcmp(command, "newwin")) {
        if (!win_create()) {
            print("Error in creating new window.\n");
        }
    } else if (!strncmp(command, "chwin", 5) && command[5] == ' ') {
        if (!win_change(atoi(next_nonblank(command + 5)))) {
            print("Error in changing window.\n");
        }
    } else if (!strcmp(command, "winstats")) {
        win_stats();
    } else if (!strcmp(command, "exit")) {
        if (!win_exit()) {
            print("There's only 1 window left. Cannot exit.\n");
        }
    } else if (!strcmp(command, "memstats")) {
        unsigned num = pages_used();
        print("Number of memory pages used is ");
        term_printi(curterm, num);
        print(".\nAnd there's ");
        term_printi(curterm, PAGENUM - num);
        print(" pages left.\n");
    } else {
        print(command);
        print(": command not found.\n");
    }
}

static char shift_escape(char c)
{
    if (c >= 'a' && c <= 'z') {
        c += 'A' - 'a';
    } else {
        switch (c) {
        case '`': c = '~'; break;
        case '1': c = '!'; break;
        case '2': c = '@'; break;
        case '3': c = '#'; break;
        case '4': c = '$'; break;
        case '5': c = '%'; break;
        case '6': c = '^'; break;
        case '7': c = '&'; break;
        case '8': c = '*'; break;
        case '9': c = '('; break;
        case '0': c = ')'; break;
        case '-': c = '_'; break;
        case '=': c = '+'; break;
        case '[': c = '{'; break;
        case ']': c = '}'; break;
        case ';': c = ';'; break;
        case '\'': c = '\"'; break;
        case ',': c = '<'; break;
        case '.': c = '>'; break;
        case '/': c = '?'; break;
        case '\\': c = '|'; break;
        default: break;
        }
    }
    return c;
}

void shell_key_handler(unsigned k)
{
    /* 获取按键的键位值 */
    char realchar = k & 0xFF;
    /* 判断 SHIFT 键 */
    if (k & LSHIFT_MASK || k & RSHIFT_MASK) {
        realchar = shift_escape(realchar);
    }
    if (term_buf_len < 63) {
        if (realchar == '\n') {
            term_buf[term_buf_len++] = '\0';
            execute(term_buf);
            if (global_key_handler == shell_key_handler) {
                term_buf_len = 0;
                print("shell >");
            }
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

static void edit_quit(bool save)
{
    if (save) {
        write_file(editing, edit_buf);
    }
    term_clear(curterm);
    init_shell();
}

void edit_key_handler(unsigned k)
{
    char ch = k & 0xFF;
    if ((k & LCTRL_MASK || k & RCTRL_MASK) && ch == 'w') {
        if (ch == 'w') {
            edit_quit(true);
            return;
        } else if (ch == 'q') {
            edit_quit(false);
            return;
        }
    }
    if (k & LSHIFT_MASK || k & RSHIFT_MASK) {
        ch = shift_escape(ch);
    }
    if (edit_buf_len < 512) {
        if (ch == '\b') {
            if (edit_buf_len > 0) {
                /* FIXME: 回退到上一行的时候光标计算会出问题 */
                term_unput(curterm);
                edit_buf[edit_buf_len] = '\0';
                --edit_buf_len;
            }
        } else if (ch == '\t') {
        } else {
            term_putchar(curterm, ch);
            edit_buf[edit_buf_len++] = ch;
        }
    }
}

void edit_mode(const char *file)
{
    term_clear(curterm);
    strncpy(editing, file, 16);
    char *content = read_file(file);
    edit_buf_len = 0;
    for (unsigned i = 0; i < strlen(content); ++i) {
        term_putchar(curterm, content[i]);
        edit_buf[edit_buf_len++] = content[i];
    }
    edit_buf[edit_buf_len] = '\0';
    global_key_handler = edit_key_handler;
}
