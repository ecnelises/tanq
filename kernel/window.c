#include "display.h"
#include <stdbool.h>

struct window *curwin;

struct window wins[4];

static unsigned win_id_counter = 1;
static unsigned win_num = 0;

void init_wins(void)
{
    for (unsigned i = 0; i < 4; ++i) {
        wins[i].id = (unsigned)~0;
        wins[i].backuped = false;
    }
}

bool win_create(void)
{
    unsigned selected = 4;
    for (unsigned i = 0; i < 4; ++i) {
        if (wins[i].id == (unsigned)~0) {
            selected = i;
            break;
        }
    }
    if (selected == 4) {
        return false;
    }
    wins[selected].id = win_id_counter++;
    curwin = wins + selected;
    term_full(&scr, &(wins[selected].tms[0]));
    curterm = &(wins[selected].tms[0]);
    ++win_num;
    return true;
}

bool win_change(unsigned id)
{
    unsigned selected = 4;
    for (unsigned i = 0; i < 4; ++i) {
        if (wins[i].id == id) {
            selected = i;
            break;
        }
    }
    if (selected == 4) {
        return false;
    }
    curwin = wins + selected;
    curterm = &(wins[selected].tms[0]);
    term_clear(curterm);
    /* TODO */
    return true;
}

void win_stats(void)
{
    print("Number of windows is ");
    term_printi(curterm, win_num);
    print("\nId of them are:\n");
    for (unsigned i = 0; i < 4; ++i) {
        if (wins[i].id != (unsigned)~0) {
            term_printi(curterm, wins[i].id);
            print("\n");
        }
    }
    print("Now we are in window ");
    term_printi(curterm, curwin->id);
    print(".\n");
}

bool win_exit(void)
{
    if (win_num == 1) {
        return false;
    }
    curwin->id = (unsigned)~0;
    for (unsigned i = 0; i < 4; ++i) {
        if (wins[i].id != (unsigned)~0) {
            win_change(wins[i].id);
            curwin = wins + i;
            curterm = &(wins[i].tms[0]);
            break;
        }
    }
    --win_num;
    return true;
}
