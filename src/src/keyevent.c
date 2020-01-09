
/*
 * iok - Virtual onscreen keyboard for Indic Inscript keymaps.
 *
 * Copyright (C) 2007 Michael Levin <risujin@risujin.org>
 * Copyright (C) 2008-2010 Parag Nemade <panemade@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 * - Added functions to handle space,backspace,tab keys like enter key handling functions.
 * - Added va(),nvav() functions definition from cellwriter's main.c and common.h
*/

#include "keys.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <gdk/gdkx.h>

/* Define this to always overwrite an unused KeyCode to send any KeySym */
/* #define ALWAYS_OVERWRITE */

extern int key_recycles, key_overwrites, key_disable_overwrite;

void key_event_cleanup(void);
void bad_keycodes_write(void);
void bad_keycodes_read(void);


/*
        X11 KeyCodes
*/

enum {
    KEY_TAKEN = 0,   /* Has KeySyms, cannot be overwritten */
    KEY_BAD,         /* Manually marked as unusable */
    KEY_USABLE,      /* Has no KeySyms, can be overwritten */
    KEY_ALLOCATED,   /* Normally usable, but currently allocated */
    /* Values greater than this represent multiple allocations */
};

char *nvav(int *plen, const char *fmt, va_list va)
{
    static char buffer[2][16000];
    static int which;
    int len;

    which = !which;
    len = g_vsnprintf(buffer[which], sizeof(buffer[which]), fmt, va);
    if (plen)
        *plen = len;
    return buffer[which];
}

char *va(const char *fmt, ...)
{
    va_list va;
    char *string;

    va_start(va, fmt);
    string = nvav(NULL, fmt, va);
    va_end(va);
    return string;
}

static char usable[256], pressed[256];
static int key_min, key_max, key_offset, key_codes;
static KeySym *keysyms;
static XModifierKeymap *modmap;

/* Bad keycodes: Despite having no KeySym entries, certain KeyCodes will
   generate special KeySyms even if their KeySym entries have been overwritten.
   For instance, KeyCode 204 attempts to eject the CD-ROM even if there is no
   CD-ROM device present! KeyCode 229 will launch GNOME file search even if
   there is no search button on the physical keyboard. There is no programatic
   way around this but to keep a list of commonly used "bad" KeyCodes. */


static void press_keycode(KeyCode k)
/* Called from various places to generate a key-down event */
{
    if (k >= key_min && k <= key_max)
        XTestFakeKeyEvent(GDK_DISPLAY(), k, True, CurrentTime);
}

static void release_keycode(KeyCode k)
/* Called from various places to generate a key-up event */
{
    if (k >= key_min && k <= key_max)
        XTestFakeKeyEvent(GDK_DISPLAY(), k, False, CurrentTime);
}


static void type_keycode(KeyCode k)
/* Key-down + key-up */
{
    press_keycode(k);
    release_keycode(k);
}

static void setup_usable(void)
/* Find unused slots in the key mapping */
{
    int i, found;

    /* Find all free keys */
    memset(usable, 0, sizeof (usable));
    for (i = key_min, found = 0; i <= key_max; i++) {
        int j;

        for (j = 0; j < key_codes &&
                keysyms[(i - key_min) * key_codes + j] == NoSymbol; j++);
        if (j < key_codes) {
            usable[i] = KEY_TAKEN;
            continue;
        }
        usable[i] = KEY_USABLE;
        found++;
    }
    key_offset = 0;

    /* If we haven't found a usable key, it's probably because we have
       already ran once and used them all up without setting them back */
    if (!found) {
        usable[key_max - key_min - 1] = KEY_USABLE;
    }
}

static void cleanup_usable(void)
/* Clear all the usable KeyCodes or we won't have any when we run again! */
{
    int i, bad, unused = 0, freed;

    for (i = 0, freed = 0, bad = 0; i <= key_max; i++)
        if (usable[i] >= KEY_USABLE) {
            int j, kc_used = FALSE;

            for (j = 0; j < key_codes; j++) {
                int index = (i - key_min) * key_codes + j;

                if (keysyms[index] != NoSymbol)
                    kc_used = TRUE;
                keysyms[index] = NoSymbol;
            }
            if (kc_used)
                freed++;
            else
                unused++;
        } else if (usable[i] == KEY_BAD)
            bad++;
    if (freed) {
        XChangeKeyboardMapping(GDK_DISPLAY(), key_min, key_codes,
                               keysyms, key_max - key_min + 1);
        XFlush(GDK_DISPLAY());
    }
    g_print("Free'd %d KeyCode(s), %d unused, %d marked bad",
            freed, unused, bad);
}

static void release_held_keys(void)
/* Release all held keys that were not pressed by us */
{
    int i;
    char keys[32];

    XQueryKeymap(GDK_DISPLAY(), keys);
    for (i = 0; i < 32; i++) {
        int j;

        for (j = 0; j < 8; j++) {
            KeyCode keycode;

            keycode = i * 8 + j;
            if (keys[i] & (1 << j) && !pressed[keycode]) {
                g_print("Released held KeyCode %d", keycode);
                release_keycode(keycode);
            }
        }
    }
}

/*
        Key Events
*/

int key_overwrites = 0, key_recycles = 0,
                                       key_shifted = 0, key_num_locked = FALSE, key_caps_locked = FALSE,
                                                                         key_disable_overwrite = FALSE;

static int alt_mask, num_lock_mask, meta_mask;
static KeyEvent ke_shift, ke_enter, ke_num_lock, ke_caps_lock,ke_tab,ke_backspace,ke_space,ke_escape;
static KeyEvent ke_f1,ke_f2,ke_f3,ke_f4,ke_f5,ke_f6,ke_f7,ke_f8,ke_f9,ke_f10,ke_f11,ke_f12;

static void reset_keyboard(void)
/* In order to reliably predict key event behavior we need to be able to
   reset the keyboard modifier and pressed state */
{
    Window root, child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    release_held_keys();
    XQueryPointer(GDK_DISPLAY(), GDK_WINDOW_XWINDOW(GDK_ROOT_PARENT()),
                  &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
    if (mask & LockMask)
        type_keycode(ke_caps_lock.keycode);
    if (mask & num_lock_mask)
        type_keycode(ke_num_lock.keycode);
}

static int is_modifier(unsigned int keysym)
/* Returns TRUE for KeySyms that are tracked internally */
{
    switch (keysym) {
    case XK_Shift_L:
    case XK_Shift_R:
    case XK_Num_Lock:
    case XK_Caps_Lock:
        return TRUE;
    default:
        return FALSE;
    }
}

static void key_event_allocate(KeyEvent *key_event, unsigned int keysym)
/* Either finds the KeyCode associated with the given keysym or overwrites
   a usable one to generate it */
{
    int i, start;

    /* Invalid KeySym */
    if (!keysym) {
        key_event->keycode = -1;
        key_event->keysym = 0;
        return;
    }

    /* First see if our KeySym is already in the mapping */
    key_event->shift = FALSE;
#ifndef ALWAYS_OVERWRITE
    for (i = 0; i <= key_max - key_min; i++) {
        if (keysyms[i * key_codes + 1] == keysym)
            key_event->shift = TRUE;
        if (keysyms[i * key_codes] == keysym || key_event->shift) {
            key_event->keycode = key_min + i;
            key_recycles++;

            /* Bump the allocation count if this is an
               allocateable KeyCode */
            if (usable[key_event->keycode] >= KEY_USABLE)
                usable[key_event->keycode]++;

            return;
        }
    }
#endif

    /* Key overwrites may be disabled, in which case we're out of luck */
    if (key_disable_overwrite) {
        key_event->keycode = -1;
        key_event->keysym = 0;
        g_print("Not allowed to overwrite KeyCode for %s",
                XKeysymToString(keysym));
        return;
    }

    /* If not, find a usable KeyCode in the mapping */
    for (start = key_offset++; ; key_offset++) {
        if (key_offset > key_max - key_min)
            key_offset = 0;
        if (usable[key_min + key_offset] == KEY_USABLE &&
                !pressed[key_min + key_offset])
            break;

        /* If we can't find one, invalidate the event */
        if (key_offset == start) {
            key_event->keycode = -1;
            key_event->keysym = 0;
            g_print("Failed to allocate KeyCode for %s",
                    XKeysymToString(keysym));
            return;
        }
    }
    key_overwrites++;
    key_event->keycode = key_min + key_offset;
    usable[key_event->keycode] = KEY_ALLOCATED;

    /* Modify the slot to hold our character */
    keysyms[key_offset * key_codes] = keysym;
    keysyms[key_offset * key_codes + 1] = keysym;
    XChangeKeyboardMapping(GDK_DISPLAY(), key_event->keycode, key_codes,
                           keysyms + key_offset * key_codes, 1);
    XSync(GDK_DISPLAY(), False);

//        g_print("Overwrote KeyCode %d for %s", key_event->keycode,XKeysymToString(keysym));
}

void key_event_new(KeyEvent *key_event, unsigned int keysym)
/* Filters locks and shifts but allocates other keys normally */
{
    key_event->keysym = keysym;
    if (is_modifier(keysym))
        return;
    key_event_allocate(key_event, keysym);
}

void key_event_free(KeyEvent *key_event)
/* Release resources associated with and invalidate a key event */
{
    if (key_event->keycode >= key_min && key_event->keycode <= key_max &&
            usable[key_event->keycode] == KEY_ALLOCATED)
        usable[key_event->keycode] = KEY_USABLE;
    key_event->keycode = -1;
    key_event->keysym = 0;
}

void key_event_press(KeyEvent *key_event)
/* Press the KeyCode specified in the event */
{
    /* Track modifiers without actually using them */
    if (key_event->keysym == XK_Shift_L ||
            key_event->keysym == XK_Shift_R) {
        key_shifted++;
        return;
    } else if (key_event->keysym == XK_Caps_Lock) {
        key_caps_locked = !key_caps_locked;
        return;
    } else if (key_event->keysym == XK_Num_Lock) {
        key_num_locked = !key_num_locked;
        return;
    }

    /* Invalid event */
    if (key_event->keycode < key_min || key_event->keycode > key_max)
        return;

    /* If this KeyCode is already pressed, something is wrong */
    if (pressed[key_event->keycode]) {
        g_print("KeyCode %d is already pressed", key_event->keycode);
        return;
    }

    /* Keep track of what KeyCodes are pressed down because of us */
    pressed[key_event->keycode] = TRUE;

    /* Reset the keyboard so we don't have to worry about locks etc */
    reset_keyboard();

    /* Press our keycode */
    if (key_event->shift)
        press_keycode(ke_shift.keycode);
    press_keycode(key_event->keycode);
    XSync(GDK_DISPLAY(), False);
}

void key_event_release(KeyEvent *key_event)
{
    /* Track modifiers without actually using them */
    if (key_event->keysym == XK_Shift_L ||
            key_event->keysym == XK_Shift_R) {
        key_shifted--;
        return;
    }

    /* Invalid key event */
    if (key_event->keycode < key_min || key_event->keycode > key_max)
        return;

    /* Keep track of what KeyCodes are pressed because of us */
    pressed[key_event->keycode] = FALSE;

    /* Release our keycode */
    release_keycode(key_event->keycode);
    if (key_event->shift)
        release_keycode(ke_shift.keycode);
    XSync(GDK_DISPLAY(), False);
}

#ifdef X_HAVE_UTF8_STRING
void key_event_send_char(int unichar)
{
    KeyEvent key_event;
    KeySym keysym;

    /* Get the keysym for the unichar (may be unsupported) */
    keysym = XStringToKeysym(va("U%04X", unichar));
    if (!keysym) {
        g_print("XStringToKeysym failed to get Keysym for '%C'",
                unichar);
        return;
    }

    key_event_new(&key_event, keysym);
    key_event_press(&key_event);
    key_event_release(&key_event);
    key_event_free(&key_event);
}
#else
#printf X_HAVE_UTF8_STRING is undefined, Unicode support is disabled!
void key_event_send_char(int unichar)
{
    KeyCode keycode;

    /* Get the keycode for an existing key */
    keycode = XKeysymToKeycode(GDK_DISPLAY(), keysym);
    if (!keycode) {
        g_print("XKeysymToKeycode failed to find KeyCode for '%C'",
                unichar);
        return;
    }

    type_keycode(keycode);
}
#endif

void key_event_send_enter()
{
    type_keycode(ke_enter.keycode);
    XSync(GDK_DISPLAY(), False);
}

void key_event_send_tab()
{
    type_keycode(ke_tab.keycode);
    XSync(GDK_DISPLAY(), False);

}
void key_event_send_space()
{
    type_keycode(ke_space.keycode);
    XSync(GDK_DISPLAY(), False);

}
void key_event_send_backspace()
{
    type_keycode(ke_backspace.keycode);
    XSync(GDK_DISPLAY(), False);
}

void key_event_send_escape()
{
    type_keycode(ke_escape.keycode);
    XSync(GDK_DISPLAY(), False);
}

void key_event_send_f1()
{

    type_keycode(ke_f1.keycode);
    XSync(GDK_DISPLAY(), False);
}

void key_event_send_f2()
{

    type_keycode(ke_f2.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f3()
{

    type_keycode(ke_f3.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f4()
{

    type_keycode(ke_f4.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f5()
{

    type_keycode(ke_f5.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f6()
{

    type_keycode(ke_f6.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f7()
{

    type_keycode(ke_f7.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f8()
{

    type_keycode(ke_f8.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f9()
{

    type_keycode(ke_f9.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f10()
{

    type_keycode(ke_f10.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f11()
{

    type_keycode(ke_f11.keycode);
    XSync(GDK_DISPLAY(), False);
}
void key_event_send_f12()
{

    type_keycode(ke_f12.keycode);
    XSync(GDK_DISPLAY(), False);
}

int key_event_init(void)
{
    int i, j, a, b, c, d;

#ifndef X_HAVE_UTF8_STRING
    g_print("Compiled without Unicode support!");
#endif

    GDK_DISPLAY() = XOpenDisplay( NULL );
    if ( GDK_DISPLAY() == NULL ) return 1;

    /* Make sure Xtest is supported */
    if (!XTestQueryExtension(GDK_DISPLAY(), &a, &b, &c, &d))
        g_print("Xtest not supported!");
    else
        g_print("Xtest version %d.%d.%d.%d\n", a, b, c, d);

    /* Get the keyboard mapping */
    XDisplayKeycodes(GDK_DISPLAY(), &key_min, &key_max);
    keysyms = XGetKeyboardMapping(GDK_DISPLAY(), key_min,
                                  key_max - key_min + 1, &key_codes);

    /* Get the modifier mapping and variable masks */
    modmap = XGetModifierMapping(GDK_DISPLAY());
    for (i = 0; i < 8; i++)
        for (j = 0; j < modmap->max_keypermod; j++) {
            KeyCode keycode;
            KeySym keysym;

            keycode = modmap->modifiermap
                      [i * modmap->max_keypermod + j];
            if (keycode < key_min || keycode > key_max)
                continue;
            keysym = keysyms[(keycode - key_min) * key_codes];
            if (keysym == XK_Alt_L || keysym == XK_Alt_R) {
                alt_mask = 1 << i;
                break;
            } else if (keysym == XK_Meta_L || keysym == XK_Meta_R) {
                meta_mask = 1 << i;
                break;
            } else if (keysym == XK_Num_Lock) {
                num_lock_mask = 1 << i;
                break;
            }
        }

    /* Clear the array that keeps track of our pressed keys */
    memset(pressed, 0, sizeof (pressed));

    /* Scan for available KeyCodes */
    setup_usable();

    /* Get some common key events */
    release_held_keys();
    key_event_allocate(&ke_shift, XK_Shift_L);
    key_event_allocate(&ke_caps_lock, XK_Caps_Lock);
    key_event_allocate(&ke_num_lock, XK_Num_Lock);
    key_event_allocate(&ke_enter, XK_Return);
    key_event_allocate(&ke_tab, XK_KP_Tab);
    key_event_allocate(&ke_backspace, XK_BackSpace);
    key_event_allocate(&ke_space, XK_KP_Space);
    key_event_allocate(&ke_escape, XK_Escape);
    key_event_allocate(&ke_f1, XK_F1);
    key_event_allocate(&ke_f2, XK_F2);
    key_event_allocate(&ke_f3, XK_F3);
    key_event_allocate(&ke_f4, XK_F4);
    key_event_allocate(&ke_f5, XK_F5);
    key_event_allocate(&ke_f6, XK_F6);
    key_event_allocate(&ke_f7, XK_F7);
    key_event_allocate(&ke_f8, XK_F8);
    key_event_allocate(&ke_f9, XK_F9);
    key_event_allocate(&ke_f10, XK_F10);
    key_event_allocate(&ke_f11, XK_F11);
    key_event_allocate(&ke_f12, XK_F12);

    return 0;
}

void key_event_cleanup(void)
{
    cleanup_usable();
}

