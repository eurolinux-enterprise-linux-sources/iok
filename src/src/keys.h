
/*
 * iok - Virtual onscreen keyboard for Indic Inscript keymaps.
 * Copyright (C) 2007 Michael Levin <risujin@risujin.org>
 * Copyright (C) 2008-2011 Parag Nemade <panemade@gmail.com>
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
 * 
*/

/*
        Key events
*/

typedef struct {
        unsigned char keycode, shift;
        unsigned int keysym;
} KeyEvent;

extern int key_shifted, key_num_locked, key_caps_locked;

void key_event_new(KeyEvent *key_event, unsigned int keysym);
void key_event_free(KeyEvent *key_event);
void key_event_press(KeyEvent *key_event);
void key_event_release(KeyEvent *key_event);
void key_event_send_char(int unichar);
void key_event_send_enter(void);
void key_event_send_backspace(void);
void key_event_send_space(void);
void key_event_send_tab(void);
int key_event_init(void);

/* Constants may not have been defined if GLib is not included */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL ((void*)0)
#endif

char *va(const char *format, ...);
