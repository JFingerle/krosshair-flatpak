#ifndef KROSSHAIR_KEYS_H
#define KROSSHAIR_KEYS_H

/*
 * Self-contained key-name -> Linux input-code table for the hotkey toggle.
 * Names follow MangoHud / XKB spellings and are matched case-insensitively.
 * No external dependency: KEY_* codes come from <linux/input.h>.
 */

#include <linux/input.h>
#include <stddef.h>

struct krosshair_key_entry {
    const char* name;
    int code;
};

static const struct krosshair_key_entry kh_key_table[] = {
    /* modifiers */
    { "shift_l", KEY_LEFTSHIFT },
    { "shift_r", KEY_RIGHTSHIFT },
    { "ctrl_l",  KEY_LEFTCTRL  },
    { "ctrl_r",  KEY_RIGHTCTRL },
    { "alt_l",   KEY_LEFTALT   },
    { "alt",     KEY_LEFTALT   },
    { "alt_r",   KEY_RIGHTALT  },
    /* letters */
    { "a", KEY_A }, { "b", KEY_B }, { "c", KEY_C }, { "d", KEY_D },
    { "e", KEY_E }, { "f", KEY_F }, { "g", KEY_G }, { "h", KEY_H },
    { "i", KEY_I }, { "j", KEY_J }, { "k", KEY_K }, { "l", KEY_L },
    { "m", KEY_M }, { "n", KEY_N }, { "o", KEY_O }, { "p", KEY_P },
    { "q", KEY_Q }, { "r", KEY_R }, { "s", KEY_S }, { "t", KEY_T },
    { "u", KEY_U }, { "v", KEY_V }, { "w", KEY_W }, { "x", KEY_X },
    { "y", KEY_Y }, { "z", KEY_Z },
    /* digits */
    { "0", KEY_0 }, { "1", KEY_1 }, { "2", KEY_2 }, { "3", KEY_3 },
    { "4", KEY_4 }, { "5", KEY_5 }, { "6", KEY_6 }, { "7", KEY_7 },
    { "8", KEY_8 }, { "9", KEY_9 },
    /* function keys */
    { "f1", KEY_F1 }, { "f2", KEY_F2 }, { "f3", KEY_F3 }, { "f4", KEY_F4 },
    { "f5", KEY_F5 }, { "f6", KEY_F6 }, { "f7", KEY_F7 }, { "f8", KEY_F8 },
    { "f9", KEY_F9 }, { "f10", KEY_F10 }, { "f11", KEY_F11 },
    { "f12", KEY_F12 }, { "f13", KEY_F13 }, { "f14", KEY_F14 },
    { "f15", KEY_F15 }, { "f16", KEY_F16 }, { "f17", KEY_F17 },
    { "f18", KEY_F18 }, { "f19", KEY_F19 }, { "f20", KEY_F20 },
    { "f21", KEY_F21 }, { "f22", KEY_F22 }, { "f23", KEY_F23 },
    { "f24", KEY_F24 },
    /* special / punctuation */
    { "escape",    KEY_ESC },
    { "space",     KEY_SPACE },
    { "return",    KEY_ENTER },
    { "tab",       KEY_TAB },
    { "backspace", KEY_BACKSPACE },
    { "delete",    KEY_DELETE },
    { "insert",    KEY_INSERT },
    { "home",      KEY_HOME },
    { "end",       KEY_END },
    { "up",        KEY_UP },
    { "down",      KEY_DOWN },
    { "left",      KEY_LEFT },
    { "right",     KEY_RIGHT },
    { "page_up",   KEY_PAGEUP },
    { "page_down", KEY_PAGEDOWN },
    { "comma",     KEY_COMMA },
    { "period",    KEY_DOT },
    { "dot",       KEY_DOT },
    { "slash",     KEY_SLASH },
    { "minus",     KEY_MINUS },
    { "equals",    KEY_EQUAL },
    { "semicolon", KEY_SEMICOLON },
    { "grave",     KEY_GRAVE },
    { NULL, -1 }
};

/* Case-insensitive lookup: returns the Linux KEY_* code, or -1 if unknown. */
static inline int kh_key_from_name(const char* s)
{
    if (!s) return -1;

    for (const struct krosshair_key_entry* e = kh_key_table; e->name; ++e) {
        size_t n = __builtin_strlen(e->name);
        const char* t = s;
        int match = 1;
        for (size_t i = 0; i < n; ++i) {
            char c = t[i];
            if (c >= 'A' && c <= 'Z') c = (char)(c + ('a' - 'A'));
            if (c != e->name[i]) {
                match = 0;
                break;
            }
        }
        if (match && t[n] == '\0') {
            return e->code;
        }
    }
    return -1;
}

#endif /* KROSSHAIR_KEYS_H */
