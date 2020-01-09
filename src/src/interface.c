
/*
 *
 * iok - Virtual onscreen keyboard for Indic Inscript keymaps.
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
 * */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "keys.h"
#include <assert.h>
#include <dirent.h>
#include <wchar.h>
#include <libgen.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)


GtkWidget *IndicMapper;
GtkWidget *vbox1;
GtkWidget *hbox0;
GtkWidget *table0;
GtkWidget *table1;
GtkWidget *table2;
GtkWidget *table3;
GtkWidget *table4;
GtkWidget *table5;
GtkWidget *table6;
GtkWidget *hbox1;
GtkWidget *btn_esc;
GtkWidget *bt_back_quote;
GtkWidget *bt_f1;
GtkWidget *bt_f2;
GtkWidget *bt_f3;
GtkWidget *bt_f4;
GtkWidget *bt_f5;
GtkWidget *bt_f6;
GtkWidget *bt_f7;
GtkWidget *bt_f8;
GtkWidget *bt_f9;
GtkWidget *bt_f10;
GtkWidget *bt_f11;
GtkWidget *bt_f12;

GtkWidget *bt_1;
GtkWidget *bt_2;
GtkWidget *bt_3;
GtkWidget *bt_4;
GtkWidget *bt_5;
GtkWidget *bt_6;
GtkWidget *bt_7;
GtkWidget *bt_8;
GtkWidget *bt_9;
GtkWidget *bt_0;
GtkWidget *bt_dash;
GtkWidget *bt_equal;
GtkWidget *backspace_btn;
GtkWidget *hbox2;
GtkWidget *tab_btn;
GtkWidget *bt_q;
GtkWidget *bt_w;
GtkWidget *bt_e;
GtkWidget *bt_r;
GtkWidget *bt_t;
GtkWidget *bt_y;
GtkWidget *bt_u;
GtkWidget *bt_i;
GtkWidget *bt_o;
GtkWidget *bt_p;
GtkWidget *bt_sq_bra_left;
GtkWidget *bt_sq_bra_right;
GtkWidget *bt_win_slash;
GtkWidget *hbox3;
GtkWidget *caps_btn;
GtkWidget *bt_a;
GtkWidget *bt_s;
GtkWidget *bt_d;
GtkWidget *bt_f;
GtkWidget *bt_g;
GtkWidget *bt_h;
GtkWidget *bt_j;
GtkWidget *bt_k;
GtkWidget *bt_l;
GtkWidget *bt_semi_colon;
GtkWidget *bt_quote;
GtkWidget *enter_btn;
GtkWidget *hbox4;
GtkWidget *l_shift_btn;
GtkWidget *bt_z;
GtkWidget *bt_x;
GtkWidget *bt_c;
GtkWidget *bt_v;
GtkWidget *bt_b;
GtkWidget *bt_n;
GtkWidget *bt_m;
GtkWidget *bt_coma;
GtkWidget *bt_dot;
GtkWidget *bt_linux_slash;
GtkWidget *r_shift_btn;
GtkWidget *hbox9;
GtkWidget *keyswitch_btn;
GtkWidget *space_btn;
GtkWidget *hbox8;
GtkWidget *bt_about;
GtkWidget *bt_wrmap;
GtkWidget *bt_reakey;
GtkWidget *bt_swapkey;
GtkWidget *combo_mim;
GtkWidget *hbox6;
int ii=0,mimcnt=0;
char *localename;
char lname[20];
char line[25];

GtkWidget *Swap_Keys;
GtkWidget *vbox1;
GtkWidget *hbox2;
GtkWidget *swap_dialog;
GtkWidget *keya;
GtkWidget *keya_entry;
GtkWidget *hbox3;
GtkWidget *keyb;
GtkWidget *keyb_entry;
GtkWidget *hbox1;
GtkWidget *btn_swap;
GtkWidget *btn_cancel_swap;
GtkWidget *slabel;
wchar_t *tmpkey;

char *gtxtkeya,*gtxtkeyb,toggle_keymap[50],map_to_read[50];
wchar_t *gtxtvaluea,*gtxtvalueb;
GtkWidget *Reassign_Key;
GtkWidget *lbl_rk1;
GtkWidget *reakeya_entry;
GtkWidget *lbl_rk2;
GtkWidget *reakeyb_entry;
GtkWidget *hbox1;
GtkWidget *btn_reassign_reakey;
GtkWidget *btn_cancel_reakey;

GtkWidget *Write_Map;
GtkWidget *lbl_wrmap1;
GtkWidget *wrmap_entry;
GtkWidget *btn_writemap;
GtkWidget *btn_cancel_wrmap;

GtkWidget *menu_bar;
GtkWidget *file_menu,*keys_menu,*help_menu,*file_item,*keys_item,*help_item;
GtkWidget *menu_items,*separator_item,*open_item,*save_item,*advopt_item,*quit_item,*reassign_item,*swap_item,*about_item;
GtkTooltips *tooltips;

char *mlinecpy,*langname;

char *row1 = "1234567890-+";
char *row2 = "qwertyuiop[]";
char *row3 = "asdfghjkl;'";
char *row4 = "<zxcvbnm,./";

gboolean enable_dnd=0;

char *maplist[50];

char *spl_sym[][2]= {
    {"apostrophe", "U0027"},
    {"asciitilde", "U007E"},
    {"exclam", "U0021"},
    {"at", "U0040"},
    {"numbersign", "U0023"},
    {"dollar", "U0024"},
    {"percent", "U0025"},
    {"asciicircum", "U005E"},
    {"ampersand", "U0026"},
    {"asterisk", "U002A"},
    {"parenleft", "U0028"},
    {"parenright", "U0029"},
    {"minus", "U002D"},
    {"underscore", "U005F"},
    {"equal", "U003D"},
    {"plus", "U002B"},
    {"grave", "U0060"},
    {"BackSpace", "UFF08"},
    {"semicolon", "U003B"},
    {"colon", "U003A"},
    {"quoteright", "U0027"},
    {"quotedbl", "U0022"},
    {"comma", "U002C"},
    {"period", "U002E"},
    {"slash", "U002F"},
    {"question", "U003F"},
    {"bracketleft", "U005B"},
    {"bracketright", "U005D"},
    {"braceleft", "U007B"},
    {"braceright", "U007D"},
    {"gur_visarga", "U0020"},
    {"less", "U003C"},
    {"greater", "U003E"},
    {NULL,NULL}
};

char *key_map[][3]= {
    {"TLDE", "`", "~"},

    {"AE01", "1", "!"},
    {"AE02", "2", "@"},
    {"AE03", "3", "#"},
    {"AE04", "4", "$"},
    {"AE05", "5", "%"},
    {"AE06", "6", "^"},
    {"AE07", "7", "&"},
    {"AE08", "8", "*"},
    {"AE09", "9", "("},
    {"AE10", "0", ")"},
    {"AE11", "-", "_"},
    {"AE12", "=", "+"},

    {"AD01", "q", "Q"},
    {"AD02", "w", "W"},
    {"AD03", "e", "E"},
    {"AD04", "r", "R"},
    {"AD05", "t", "T"},
    {"AD06", "y", "Y"},
    {"AD07", "u", "U"},
    {"AD08", "i", "I"},
    {"AD09", "o", "O"},
    {"AD10", "p", "P"},
    {"AD11", "[", "{"},
    {"AD12", "]", "}"},
    {"BKSL", "|", "\\"},

    {"AC01", "a", "A"},
    {"AC02", "s", "S"},
    {"AC03", "d", "D"},
    {"AC04", "f", "F"},
    {"AC05", "g", "G"},
    {"AC06", "h", "H"},
    {"AC07", "j", "J"},
    {"AC08", "k", "K"},
    {"AC09", "l", "L"},
    {"AC10", ";", ":"},
    {"AC11", "'", "\""},

    {"AB01", "z", "Z"},
    {"AB02", "x", "X"},
    {"AB03", "c", "C"},
    {"AB04", "v", "V"},
    {"AB05", "b", "B"},
    {"AB06", "n", "N"},
    {"AB07", "m", "M"},
    {"AB08", ",", "<"},
    {"AB09", ".", ">"},
    {"AB10", "/", "?"},

    {NULL,NULL}
};

typedef struct
{
    char keyname[1];
    wchar_t keyvalue[3];
} Mappings;


Mappings map[95];

typedef struct
{
    char xkeynamel[1];
    char xkeynameu[1];
    char xkeyvaluel[15];
    char xkeyvalueu[15];
} xMappings;


xMappings xmap[50];

int mapcnt=0,xmapcnt=0;

char* srch(char *searchkey)
{
    int nitems=mapcnt;
    int jj=0;
    for (jj=0; jj<nitems; jj++)
        if (strcmp(map[jj].keyname,searchkey)==0) {
            if (strcmp((gchar *)map[jj].keyvalue,"‌")==0)
                return "ZWNJ";
            else if (strcmp((gchar *)map[jj].keyvalue,"‍")==0)
                return "ZWJ";
            else
                return map[jj].keyvalue;
        }
    return "";
}

void clear_mappings()
{
    int clr=0;
    for (clr=0; clr<94; clr++)
    {
        memset(map[clr].keyname,'\0',1);
        wmemset(map[clr].keyvalue,'\0',3);
    }

    for (clr=0; clr<50; clr++)
    {
        memset(xmap[clr].xkeynamel,'\0',1);
        memset(xmap[clr].xkeyvaluel,'\0',1);
        memset(xmap[clr].xkeynameu,'\0',15);
        memset(xmap[clr].xkeyvalueu,'\0',15);
    }

    mapcnt=0;
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_back_quote,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_1,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_2,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_3,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_4,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_5,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_6,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_7,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_8,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_9,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_0,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_dash,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_equal,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_q,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_w,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_e,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_r,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_t,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_y,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_u,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_i,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_o,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_p,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_sq_bra_left,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_sq_bra_right,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_win_slash,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_a,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_s,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_d,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_f,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_g,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_h,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_j,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_k,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_l,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_semi_colon,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_quote,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_z,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_x,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_c,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_v,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_b,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_n,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_m,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_coma,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_dot,lname);
    strcpy(lname,"");
    gtk_button_set_label((GtkButton *)bt_linux_slash,lname);

}

void load_shift_mappings(int whichkey)
{
    if (whichkey)
    {
        sprintf(lname,"%s",srch("~"));
        gtk_button_set_label((GtkButton *)bt_back_quote,lname);
        sprintf(lname,"%s",srch("!"));
        gtk_button_set_label((GtkButton *)bt_1,lname);
        sprintf(lname,"%s",srch("@"));
        gtk_button_set_label((GtkButton *)bt_2,lname);
        sprintf(lname,"%s",srch("#"));
        gtk_button_set_label((GtkButton *)bt_3,lname);
        sprintf(lname,"%s",srch("$"));
        gtk_button_set_label((GtkButton *)bt_4,lname);
        sprintf(lname,"%s",srch("%"));
        gtk_button_set_label((GtkButton *)bt_5,lname);
        sprintf(lname,"%s",srch("^"));
        gtk_button_set_label((GtkButton *)bt_6,lname);
        sprintf(lname,"%s",srch("&"));
        gtk_button_set_label((GtkButton *)bt_7,lname);
        sprintf(lname,"%s",srch("*"));
        gtk_button_set_label((GtkButton *)bt_8,lname);
        sprintf(lname,"%s",srch("("));
        gtk_button_set_label((GtkButton *)bt_9,lname);
        sprintf(lname,"%s",srch(")"));
        gtk_button_set_label((GtkButton *)bt_0,lname);
        sprintf(lname,"%s",srch("_"));
        gtk_button_set_label((GtkButton *)bt_dash,lname);
        sprintf(lname,"%s",srch("+"));
        gtk_button_set_label((GtkButton *)bt_equal,lname);
    }
    else
    {
        sprintf(lname,"%s",srch("`"));
        gtk_button_set_label((GtkButton *)bt_back_quote,lname);
        sprintf(lname,"%s",srch("1"));
        gtk_button_set_label((GtkButton *)bt_1,lname);
        sprintf(lname,"%s",srch("2"));
        gtk_button_set_label((GtkButton *)bt_2,lname);
        sprintf(lname,"%s",srch("3"));
        gtk_button_set_label((GtkButton *)bt_3,lname);
        sprintf(lname,"%s",srch("4"));
        gtk_button_set_label((GtkButton *)bt_4,lname);
        sprintf(lname,"%s",srch("5"));
        gtk_button_set_label((GtkButton *)bt_5,lname);
        sprintf(lname,"%s",srch("6"));
        gtk_button_set_label((GtkButton *)bt_6,lname);
        sprintf(lname,"%s",srch("7"));
        gtk_button_set_label((GtkButton *)bt_7,lname);
        sprintf(lname,"%s",srch("8"));
        gtk_button_set_label((GtkButton *)bt_8,lname);
        sprintf(lname,"%s",srch("9"));
        gtk_button_set_label((GtkButton *)bt_9,lname);
        sprintf(lname,"%s",srch("0"));
        gtk_button_set_label((GtkButton *)bt_0,lname);
        sprintf(lname,"%s",srch("-"));
        gtk_button_set_label((GtkButton *)bt_dash,lname);
        sprintf(lname,"%s",srch("="));
        gtk_button_set_label((GtkButton *)bt_equal,lname);
    }
    sprintf(lname,"%s",srch("Q"));
    gtk_button_set_label((GtkButton *)bt_q,lname);
    sprintf(lname,"%s",srch("W"));
    gtk_button_set_label((GtkButton *)bt_w,lname);
    sprintf(lname,"%s",srch("E"));
    gtk_button_set_label((GtkButton *)bt_e,lname);
    sprintf(lname,"%s",srch("R"));
    gtk_button_set_label((GtkButton *)bt_r,lname);
    sprintf(lname,"%s",srch("T"));
    gtk_button_set_label((GtkButton *)bt_t,lname);
    sprintf(lname,"%s",srch("Y"));
    gtk_button_set_label((GtkButton *)bt_y,lname);
    sprintf(lname,"%s",srch("U"));
    gtk_button_set_label((GtkButton *)bt_u,lname);
    sprintf(lname,"%s",srch("I"));
    gtk_button_set_label((GtkButton *)bt_i,lname);
    sprintf(lname,"%s",srch("O"));
    gtk_button_set_label((GtkButton *)bt_o,lname);
    sprintf(lname,"%s",srch("P"));
    gtk_button_set_label((GtkButton *)bt_p,lname);
    sprintf(lname,"%s",srch("{"));
    gtk_button_set_label((GtkButton *)bt_sq_bra_left,lname);
    sprintf(lname,"%s",srch("}"));
    gtk_button_set_label((GtkButton *)bt_sq_bra_right,lname);
    sprintf(lname,"%s",srch("|"));
    gtk_button_set_label((GtkButton *)bt_win_slash,lname);
    sprintf(lname,"%s",srch("A"));
    gtk_button_set_label((GtkButton *)bt_a,lname);
    sprintf(lname,"%s",srch("S"));
    gtk_button_set_label((GtkButton *)bt_s,lname);
    sprintf(lname,"%s",srch("D"));
    gtk_button_set_label((GtkButton *)bt_d,lname);
    sprintf(lname,"%s",srch("F"));
    gtk_button_set_label((GtkButton *)bt_f,lname);
    sprintf(lname,"%s",srch("G"));
    gtk_button_set_label((GtkButton *)bt_g,lname);
    sprintf(lname,"%s",srch("H"));
    gtk_button_set_label((GtkButton *)bt_h,lname);
    sprintf(lname,"%s",srch("J"));
    gtk_button_set_label((GtkButton *)bt_j,lname);
    sprintf(lname,"%s",srch("K"));
    gtk_button_set_label((GtkButton *)bt_k,lname);
    sprintf(lname,"%s",srch("L"));
    gtk_button_set_label((GtkButton *)bt_l,lname);
    sprintf(lname,"%s",srch(":"));
    gtk_button_set_label((GtkButton *)bt_semi_colon,lname);
    sprintf(lname,"%s",srch("\""));
    gtk_button_set_label((GtkButton *)bt_quote,lname);
    sprintf(lname,"%s",srch("Z"));
    gtk_button_set_label((GtkButton *)bt_z,lname);
    sprintf(lname,"%s",srch("X"));
    gtk_button_set_label((GtkButton *)bt_x,lname);
    sprintf(lname,"%s",srch("C"));
    gtk_button_set_label((GtkButton *)bt_c,lname);
    sprintf(lname,"%s",srch("V"));
    gtk_button_set_label((GtkButton *)bt_v,lname);
    sprintf(lname,"%s",srch("B"));
    gtk_button_set_label((GtkButton *)bt_b,lname);
    sprintf(lname,"%s",srch("N"));
    gtk_button_set_label((GtkButton *)bt_n,lname);
    sprintf(lname,"%s",srch("M"));
    gtk_button_set_label((GtkButton *)bt_m,lname);
    sprintf(lname,"%s",srch("<"));
    gtk_button_set_label((GtkButton *)bt_coma,lname);
    sprintf(lname,"%s",srch(">"));
    gtk_button_set_label((GtkButton *)bt_dot,lname);
    sprintf(lname,"%s",srch("?"));
    gtk_button_set_label((GtkButton *)bt_linux_slash,lname);

}
void load_mappings()
{
    sprintf(lname,"%s",srch("`"));
    gtk_button_set_label((GtkButton *)bt_back_quote,lname);
    sprintf(lname,"%s",srch("1"));
    gtk_button_set_label((GtkButton *)bt_1,lname);
    sprintf(lname,"%s",srch("2"));
    gtk_button_set_label((GtkButton *)bt_2,lname);
    sprintf(lname,"%s",srch("3"));
    gtk_button_set_label((GtkButton *)bt_3,lname);
    sprintf(lname,"%s",srch("4"));
    gtk_button_set_label((GtkButton *)bt_4,lname);
    sprintf(lname,"%s",srch("5"));
    gtk_button_set_label((GtkButton *)bt_5,lname);
    sprintf(lname,"%s",srch("6"));
    gtk_button_set_label((GtkButton *)bt_6,lname);
    sprintf(lname,"%s",srch("7"));
    gtk_button_set_label((GtkButton *)bt_7,lname);
    sprintf(lname,"%s",srch("8"));
    gtk_button_set_label((GtkButton *)bt_8,lname);
    sprintf(lname,"%s",srch("9"));
    gtk_button_set_label((GtkButton *)bt_9,lname);
    sprintf(lname,"%s",srch("0"));
    gtk_button_set_label((GtkButton *)bt_0,lname);
    sprintf(lname,"%s",srch("-"));
    gtk_button_set_label((GtkButton *)bt_dash,lname);
    sprintf(lname,"%s",srch("="));
    gtk_button_set_label((GtkButton *)bt_equal,lname);
    sprintf(lname,"%s",srch("q"));
    gtk_button_set_label((GtkButton *)bt_q,lname);
    sprintf(lname,"%s",srch("w"));
    gtk_button_set_label((GtkButton *)bt_w,lname);
    sprintf(lname,"%s",srch("e"));
    gtk_button_set_label((GtkButton *)bt_e,lname);
    sprintf(lname,"%s",srch("r"));
    gtk_button_set_label((GtkButton *)bt_r,lname);
    sprintf(lname,"%s",srch("t"));
    gtk_button_set_label((GtkButton *)bt_t,lname);
    sprintf(lname,"%s",srch("y"));
    gtk_button_set_label((GtkButton *)bt_y,lname);
    sprintf(lname,"%s",srch("u"));
    gtk_button_set_label((GtkButton *)bt_u,lname);
    sprintf(lname,"%s",srch("i"));
    gtk_button_set_label((GtkButton *)bt_i,lname);
    sprintf(lname,"%s",srch("o"));
    gtk_button_set_label((GtkButton *)bt_o,lname);
    sprintf(lname,"%s",srch("p"));
    gtk_button_set_label((GtkButton *)bt_p,lname);
    sprintf(lname,"%s",srch("["));
    gtk_button_set_label((GtkButton *)bt_sq_bra_left,lname);
    sprintf(lname,"%s",srch("]"));
    gtk_button_set_label((GtkButton *)bt_sq_bra_right,lname);
    sprintf(lname,"%s",srch("\\"));
    gtk_button_set_label((GtkButton *)bt_win_slash,lname);
    sprintf(lname,"%s",srch("a"));
    gtk_button_set_label((GtkButton *)bt_a,lname);
    sprintf(lname,"%s",srch("s"));
    gtk_button_set_label((GtkButton *)bt_s,lname);
    sprintf(lname,"%s",srch("d"));
    gtk_button_set_label((GtkButton *)bt_d,lname);
    sprintf(lname,"%s",srch("f"));
    gtk_button_set_label((GtkButton *)bt_f,lname);
    sprintf(lname,"%s",srch("g"));
    gtk_button_set_label((GtkButton *)bt_g,lname);
    sprintf(lname,"%s",srch("h"));
    gtk_button_set_label((GtkButton *)bt_h,lname);
    sprintf(lname,"%s",srch("j"));
    gtk_button_set_label((GtkButton *)bt_j,lname);
    sprintf(lname,"%s",srch("k"));
    gtk_button_set_label((GtkButton *)bt_k,lname);
    sprintf(lname,"%s",srch("l"));
    gtk_button_set_label((GtkButton *)bt_l,lname);
    sprintf(lname,"%s",srch(";"));
    gtk_button_set_label((GtkButton *)bt_semi_colon,lname);
    sprintf(lname,"%s",srch("'"));
    gtk_button_set_label((GtkButton *)bt_quote,lname);
    sprintf(lname,"%s",srch("z"));
    gtk_button_set_label((GtkButton *)bt_z,lname);
    sprintf(lname,"%s",srch("x"));
    gtk_button_set_label((GtkButton *)bt_x,lname);
    sprintf(lname,"%s",srch("c"));
    gtk_button_set_label((GtkButton *)bt_c,lname);
    sprintf(lname,"%s",srch("v"));
    gtk_button_set_label((GtkButton *)bt_v,lname);
    sprintf(lname,"%s",srch("b"));
    gtk_button_set_label((GtkButton *)bt_b,lname);
    sprintf(lname,"%s",srch("n"));
    gtk_button_set_label((GtkButton *)bt_n,lname);
    sprintf(lname,"%s",srch("m"));
    gtk_button_set_label((GtkButton *)bt_m,lname);
    sprintf(lname,"%s",srch(","));
    gtk_button_set_label((GtkButton *)bt_coma,lname);
    sprintf(lname,"%s",srch("."));
    gtk_button_set_label((GtkButton *)bt_dot,lname);
    sprintf(lname,"%s",srch("/"));
    gtk_button_set_label((GtkButton *)bt_linux_slash,lname);

}

void english_shift_mappings(int whichkey)
{
    if (whichkey)
    {
        strcpy(lname,"~");
        gtk_button_set_label((GtkButton *)bt_back_quote,lname);
        strcpy(lname,"!");
        gtk_button_set_label((GtkButton *)bt_1,lname);
        strcpy(lname,"@");
        gtk_button_set_label((GtkButton *)bt_2,lname);
        strcpy(lname,"#");
        gtk_button_set_label((GtkButton *)bt_3,lname);
        strcpy(lname,"$");
        gtk_button_set_label((GtkButton *)bt_4,lname);
        strcpy(lname,"%");
        gtk_button_set_label((GtkButton *)bt_5,lname);
        strcpy(lname,"^");
        gtk_button_set_label((GtkButton *)bt_6,lname);
        strcpy(lname,"&");
        gtk_button_set_label((GtkButton *)bt_7,lname);
        strcpy(lname,"*");
        gtk_button_set_label((GtkButton *)bt_8,lname);
        strcpy(lname,"(");
        gtk_button_set_label((GtkButton *)bt_9,lname);
        strcpy(lname,")");
        gtk_button_set_label((GtkButton *)bt_0,lname);
        strcpy(lname,"_");
        gtk_button_set_label((GtkButton *)bt_dash,lname);
        strcpy(lname,"+");
        gtk_button_set_label((GtkButton *)bt_equal,lname);
    }
    else
    {
        strcpy(lname,"`");
        gtk_button_set_label((GtkButton *)bt_back_quote,lname);
        strcpy(lname,"1");
        gtk_button_set_label((GtkButton *)bt_1,lname);
        strcpy(lname,"2");
        gtk_button_set_label((GtkButton *)bt_2,lname);
        strcpy(lname,"3");
        gtk_button_set_label((GtkButton *)bt_3,lname);
        strcpy(lname,"4");
        gtk_button_set_label((GtkButton *)bt_4,lname);
        strcpy(lname,"5");
        gtk_button_set_label((GtkButton *)bt_5,lname);
        strcpy(lname,"6");
        gtk_button_set_label((GtkButton *)bt_6,lname);
        strcpy(lname,"7");
        gtk_button_set_label((GtkButton *)bt_7,lname);
        strcpy(lname,"8");
        gtk_button_set_label((GtkButton *)bt_8,lname);
        strcpy(lname,"9");
        gtk_button_set_label((GtkButton *)bt_9,lname);
        strcpy(lname,"0");
        gtk_button_set_label((GtkButton *)bt_0,lname);
        strcpy(lname,"-");
        gtk_button_set_label((GtkButton *)bt_dash,lname);
        strcpy(lname,"=");
        gtk_button_set_label((GtkButton *)bt_equal,lname);
    }
    strcpy(lname,"Q");
    gtk_button_set_label((GtkButton *)bt_q,lname);
    strcpy(lname,"W");
    gtk_button_set_label((GtkButton *)bt_w,lname);
    strcpy(lname,"E");
    gtk_button_set_label((GtkButton *)bt_e,lname);
    strcpy(lname,"R");
    gtk_button_set_label((GtkButton *)bt_r,lname);
    strcpy(lname,"T");
    gtk_button_set_label((GtkButton *)bt_t,lname);
    strcpy(lname,"Y");
    gtk_button_set_label((GtkButton *)bt_y,lname);
    strcpy(lname,"U");
    gtk_button_set_label((GtkButton *)bt_u,lname);
    strcpy(lname,"I");
    gtk_button_set_label((GtkButton *)bt_i,lname);
    strcpy(lname,"O");
    gtk_button_set_label((GtkButton *)bt_o,lname);
    strcpy(lname,"P");
    gtk_button_set_label((GtkButton *)bt_p,lname);
    strcpy(lname,"{");
    gtk_button_set_label((GtkButton *)bt_sq_bra_left,lname);
    strcpy(lname,"}");
    gtk_button_set_label((GtkButton *)bt_sq_bra_right,lname);
    strcpy(lname,"|");
    gtk_button_set_label((GtkButton *)bt_win_slash,lname);
    strcpy(lname,"A");
    gtk_button_set_label((GtkButton *)bt_a,lname);
    strcpy(lname,"S");
    gtk_button_set_label((GtkButton *)bt_s,lname);
    strcpy(lname,"D");
    gtk_button_set_label((GtkButton *)bt_d,lname);
    strcpy(lname,"F");
    gtk_button_set_label((GtkButton *)bt_f,lname);
    strcpy(lname,"G");
    gtk_button_set_label((GtkButton *)bt_g,lname);
    strcpy(lname,"H");
    gtk_button_set_label((GtkButton *)bt_h,lname);
    strcpy(lname,"J");
    gtk_button_set_label((GtkButton *)bt_j,lname);
    strcpy(lname,"K");
    gtk_button_set_label((GtkButton *)bt_k,lname);
    strcpy(lname,"L");
    gtk_button_set_label((GtkButton *)bt_l,lname);
    strcpy(lname,":");
    gtk_button_set_label((GtkButton *)bt_semi_colon,lname);
    strcpy(lname,"\"");
    gtk_button_set_label((GtkButton *)bt_quote,lname);
    strcpy(lname,"Z");
    gtk_button_set_label((GtkButton *)bt_z,lname);
    strcpy(lname,"X");
    gtk_button_set_label((GtkButton *)bt_x,lname);
    strcpy(lname,"C");
    gtk_button_set_label((GtkButton *)bt_c,lname);
    strcpy(lname,"V");
    gtk_button_set_label((GtkButton *)bt_v,lname);
    strcpy(lname,"B");
    gtk_button_set_label((GtkButton *)bt_b,lname);
    strcpy(lname,"N");
    gtk_button_set_label((GtkButton *)bt_n,lname);
    strcpy(lname,"M");
    gtk_button_set_label((GtkButton *)bt_m,lname);
    strcpy(lname,"<");
    gtk_button_set_label((GtkButton *)bt_coma,lname);
    strcpy(lname,">");
    gtk_button_set_label((GtkButton *)bt_dot,lname);
    strcpy(lname,"?");
    gtk_button_set_label((GtkButton *)bt_linux_slash,lname);
}

void english_mappings()
{
    mapcnt=0;
    strcpy(lname,"`");
    gtk_button_set_label((GtkButton *)bt_back_quote,lname);
    strncpy(map[mapcnt].keyname,"~",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"~");
    strncpy(map[mapcnt].keyname,"`",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"`");

    strcpy(lname,"1");
    gtk_button_set_label((GtkButton *)bt_1,lname);
    strncpy(map[mapcnt].keyname,"!",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"!");
    strncpy(map[mapcnt].keyname,"1",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"1");

    strcpy(lname,"2");
    gtk_button_set_label((GtkButton *)bt_2,lname);
    strncpy(map[mapcnt].keyname,"@",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"@");
    strncpy(map[mapcnt].keyname,"2",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"2");

    strcpy(lname,"3");
    gtk_button_set_label((GtkButton *)bt_3,lname);
    strncpy(map[mapcnt].keyname,"#",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"#");
    strncpy(map[mapcnt].keyname,"3",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"3");

    strcpy(lname,"4");
    gtk_button_set_label((GtkButton *)bt_4,lname);
    strncpy(map[mapcnt].keyname,"$",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"$");
    strncpy(map[mapcnt].keyname,"4",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"4");

    strcpy(lname,"5");
    gtk_button_set_label((GtkButton *)bt_5,lname);
    strncpy(map[mapcnt].keyname,"%",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"%");
    strncpy(map[mapcnt].keyname,"5",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"5");

    strcpy(lname,"6");
    gtk_button_set_label((GtkButton *)bt_6,lname);
    strncpy(map[mapcnt].keyname,"^",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"^");
    strncpy(map[mapcnt].keyname,"6",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"6");

    strcpy(lname,"7");
    gtk_button_set_label((GtkButton *)bt_7,lname);
    strncpy(map[mapcnt].keyname,"&",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"&");
    strncpy(map[mapcnt].keyname,"7",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"7");

    strcpy(lname,"8");
    gtk_button_set_label((GtkButton *)bt_8,lname);
    strncpy(map[mapcnt].keyname,"*",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"*");
    strncpy(map[mapcnt].keyname,"8",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"8");

    strcpy(lname,"9");
    gtk_button_set_label((GtkButton *)bt_9,lname);
    strncpy(map[mapcnt].keyname,"(",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"(");
    strncpy(map[mapcnt].keyname,"9",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"9");

    strcpy(lname,"0");
    gtk_button_set_label((GtkButton *)bt_0,lname);
    strncpy(map[mapcnt].keyname,")",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,")");
    strncpy(map[mapcnt].keyname,"0",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"0");

    strcpy(lname,"-");
    gtk_button_set_label((GtkButton *)bt_dash,lname);
    strncpy(map[mapcnt].keyname,"_",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"_");
    strncpy(map[mapcnt].keyname,"-",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"-");

    strcpy(lname,"=");
    gtk_button_set_label((GtkButton *)bt_equal,lname);
    strncpy(map[mapcnt].keyname,"+",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"+");
    strncpy(map[mapcnt].keyname,"=",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"=");

    strcpy(lname,"q");
    gtk_button_set_label((GtkButton *)bt_q,lname);
    strncpy(map[mapcnt].keyname,"Q",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"Q");
    strncpy(map[mapcnt].keyname,"q",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"q");

    strcpy(lname,"w");
    gtk_button_set_label((GtkButton *)bt_w,lname);
    strncpy(map[mapcnt].keyname,"W",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"W");
    strncpy(map[mapcnt].keyname,"w",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"w");

    strcpy(lname,"e");
    gtk_button_set_label((GtkButton *)bt_e,lname);
    strncpy(map[mapcnt].keyname,"E",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"E");
    strncpy(map[mapcnt].keyname,"e",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"e");

    strcpy(lname,"r");
    gtk_button_set_label((GtkButton *)bt_r,lname);
    strncpy(map[mapcnt].keyname,"R",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"R");
    strncpy(map[mapcnt].keyname,"r",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"r");

    strcpy(lname,"t");
    gtk_button_set_label((GtkButton *)bt_t,lname);
    strncpy(map[mapcnt].keyname,"T",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"T");
    strncpy(map[mapcnt].keyname,"t",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"t");

    strcpy(lname,"y");
    gtk_button_set_label((GtkButton *)bt_y,lname);
    strncpy(map[mapcnt].keyname,"Y",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"Y");
    strncpy(map[mapcnt].keyname,"y",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"y");

    strcpy(lname,"u");
    gtk_button_set_label((GtkButton *)bt_u,lname);
    strncpy(map[mapcnt].keyname,"U",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"U");
    strncpy(map[mapcnt].keyname,"u",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"u");

    strcpy(lname,"i");
    gtk_button_set_label((GtkButton *)bt_i,lname);
    strncpy(map[mapcnt].keyname,"I",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"I");
    strncpy(map[mapcnt].keyname,"i",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"i");

    strcpy(lname,"o");
    gtk_button_set_label((GtkButton *)bt_o,lname);
    strncpy(map[mapcnt].keyname,"O",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"O");
    strncpy(map[mapcnt].keyname,"o",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"o");

    strcpy(lname,"p");
    gtk_button_set_label((GtkButton *)bt_p,lname);
    strncpy(map[mapcnt].keyname,"P",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"P");
    strncpy(map[mapcnt].keyname,"p",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"p");

    strcpy(lname,"[");
    gtk_button_set_label((GtkButton *)bt_sq_bra_left,lname);
    strncpy(map[mapcnt].keyname,"{",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"{");
    strncpy(map[mapcnt].keyname,"[",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"[");

    strcpy(lname,"]");
    gtk_button_set_label((GtkButton *)bt_sq_bra_right,lname);
    strncpy(map[mapcnt].keyname,"}",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"}");
    strncpy(map[mapcnt].keyname,"]",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"]");

    strcpy(lname,"\\");
    gtk_button_set_label((GtkButton *)bt_win_slash,lname);
    strncpy(map[mapcnt].keyname,"|",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"|");
    strncpy(map[mapcnt].keyname,"\\",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"\\");

    strcpy(lname,"a");
    gtk_button_set_label((GtkButton *)bt_a,lname);
    strncpy(map[mapcnt].keyname,"A",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"A");
    strncpy(map[mapcnt].keyname,"a",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"a");

    strcpy(lname,"s");
    gtk_button_set_label((GtkButton *)bt_s,lname);
    strncpy(map[mapcnt].keyname,"S",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"S");
    strncpy(map[mapcnt].keyname,"s",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"s");

    strcpy(lname,"d");
    gtk_button_set_label((GtkButton *)bt_d,lname);
    strncpy(map[mapcnt].keyname,"D",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"D");
    strncpy(map[mapcnt].keyname,"d",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"d");

    strcpy(lname,"f");
    gtk_button_set_label((GtkButton *)bt_f,lname);
    strncpy(map[mapcnt].keyname,"F",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"F");
    strncpy(map[mapcnt].keyname,"f",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"f");

    strcpy(lname,"g");
    gtk_button_set_label((GtkButton *)bt_g,lname);
    strncpy(map[mapcnt].keyname,"G",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"G");
    strncpy(map[mapcnt].keyname,"g",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"g");

    strcpy(lname,"h");
    gtk_button_set_label((GtkButton *)bt_h,lname);
    strncpy(map[mapcnt].keyname,"H",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"H");
    strncpy(map[mapcnt].keyname,"h",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"h");

    strcpy(lname,"j");
    gtk_button_set_label((GtkButton *)bt_j,lname);
    strncpy(map[mapcnt].keyname,"J",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"J");
    strncpy(map[mapcnt].keyname,"j",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"j");

    strcpy(lname,"k");
    gtk_button_set_label((GtkButton *)bt_k,lname);
    strncpy(map[mapcnt].keyname,"K",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"K");
    strncpy(map[mapcnt].keyname,"k",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"k");

    strcpy(lname,"l");
    gtk_button_set_label((GtkButton *)bt_l,lname);
    strncpy(map[mapcnt].keyname,"L",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"L");
    strncpy(map[mapcnt].keyname,"l",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"l");

    strcpy(lname,";");
    gtk_button_set_label((GtkButton *)bt_semi_colon,lname);
    strncpy(map[mapcnt].keyname,":",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,":");
    strncpy(map[mapcnt].keyname,";",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,";");

    strcpy(lname,"'");
    gtk_button_set_label((GtkButton *)bt_quote,lname);
    strncpy(map[mapcnt].keyname,"\"",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"\"");
    strncpy(map[mapcnt].keyname,"'",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"'");

    strcpy(lname,"z");
    gtk_button_set_label((GtkButton *)bt_z,lname);
    strncpy(map[mapcnt].keyname,"Z",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"Z");
    strncpy(map[mapcnt].keyname,"z",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"z");

    strcpy(lname,"x");
    gtk_button_set_label((GtkButton *)bt_x,lname);
    strncpy(map[mapcnt].keyname,"X",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"X");
    strncpy(map[mapcnt].keyname,"x",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"x");

    strcpy(lname,"c");
    gtk_button_set_label((GtkButton *)bt_c,lname);
    strncpy(map[mapcnt].keyname,"C",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"C");
    strncpy(map[mapcnt].keyname,"c",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"c");

    strcpy(lname,"v");
    gtk_button_set_label((GtkButton *)bt_v,lname);
    strncpy(map[mapcnt].keyname,"V",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"V");
    strncpy(map[mapcnt].keyname,"v",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"v");

    strcpy(lname,"b");
    gtk_button_set_label((GtkButton *)bt_b,lname);
    strncpy(map[mapcnt].keyname,"B",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"B");
    strncpy(map[mapcnt].keyname,"b",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"b");

    strcpy(lname,"n");
    gtk_button_set_label((GtkButton *)bt_n,lname);
    strncpy(map[mapcnt].keyname,"N",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"N");
    strncpy(map[mapcnt].keyname,"n",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"n");

    strcpy(lname,"m");
    gtk_button_set_label((GtkButton *)bt_m,lname);
    strncpy(map[mapcnt].keyname,"M",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"M");
    strncpy(map[mapcnt].keyname,"m",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"m");

    strcpy(lname,",");
    gtk_button_set_label((GtkButton *)bt_coma,lname);
    strncpy(map[mapcnt].keyname,"<",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"<");
    strncpy(map[mapcnt].keyname,",",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,",");

    strcpy(lname,".");
    gtk_button_set_label((GtkButton *)bt_dot,lname);
    strncpy(map[mapcnt].keyname,">",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,">");
    strncpy(map[mapcnt].keyname,".",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,".");

    strcpy(lname,"/");
    gtk_button_set_label((GtkButton *)bt_linux_slash,lname);
    strncpy(map[mapcnt].keyname,"?",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"?");
    strncpy(map[mapcnt].keyname,"/",1);
    strcpy((gchar *)map[mapcnt++].keyvalue,"/");
}

void get_xkb_mappings(char *mfile)
{
    FILE *ifp;
    int i=0,j=0;
    char iline[80];
    char mmfile[80];
    char *ntoken;
    char *itoken;

    strcpy(mmfile,mfile+4);
    xmapcnt=0;
    ifp= fopen("/usr/share/X11/xkb/symbols/in", "r");
    while (fgets(iline,80,ifp) != NULL)
    {
        if (strstr(iline,"xkb_symbols")!=NULL)
        {
            ntoken=(char *) malloc(sizeof(char) * 25);
            memset(ntoken,'\0',25);
            ntoken = strtok(iline, " ");
            ntoken = strtok(NULL, " \"");
        }

        if (strstr(iline,"name[Group1]")!=NULL)
        {
            if (strcmp(iline,"\tname[Group1]=\"India\";\n")==0)
            {
                itoken=(char *) malloc(sizeof(char) * 7);
                memset(itoken,'\0',7);
                strcpy(itoken,"India");
            }
            else
            {
                char *nntoken;
                size_t nn=0,nn_cnt=0;
                itoken=(char *)malloc(65*sizeof(char));
                memset(itoken,'\0',65);

                nn=strlen(iline);
                while (nn_cnt < nn)
                {
                    if (iline[nn_cnt] == '\"')
                    {
                        strncpy(itoken,iline+nn_cnt+1,nn-nn_cnt-4);
                        break;
                    }
                    else
                        nn_cnt++;
                }
            }

            //Start actual parsing of map
            if (strcmp(itoken,mmfile)==0 )
            {
                while (fgets(iline,80,ifp) != NULL)
                {
                    if (strcmp(iline,"};\n")==0)
                        break;
                    else
                    {
                        char *ktoken,*ktokenl,*ktokenu,*ktokenn;
                        if (strstr(iline,"key <")!=NULL && iline[0]!='/' && strstr(iline,"RALT")==NULL)
                        {
                            i=0;
                            ktoken = strtok(iline, " \t");
                            ktokenn = strtok(NULL, " <>");
                            ktoken = strtok(NULL, " \t{}");
                            ktokenl = strtok(NULL, " ,\t");
                            if (strstr(ktokenl,"U")==NULL )
                            {
                                for (j=0; j<33; j++)
                                    if (strcmp(ktokenl,spl_sym[j][0])==0) {
                                        strcpy(ktokenl,spl_sym[j][1]);
                                    }
                            }

                            ktokenu = strtok(NULL, " ,\t]\\");
                            if (strstr(ktokenu,"U")==NULL )
                            {
                                for (j=0; j<33; j++)
                                    if (strcmp(ktokenu,spl_sym[j][0])==0) {
                                        strcpy(ktokenu,spl_sym[j][1]);
                                    }
                            }

                            if (strstr(ktokenn,"AE")!=NULL || strstr(ktokenn,"AD")!=NULL || strstr(ktokenn,"BKSL")!=NULL || strstr(ktokenn,"AC")!=NULL || (strstr(ktokenn,"AB")!=NULL && strstr(ktokenn,"TAB")==NULL ) || strstr(ktokenn,"TLDE")!=NULL )
                            {
                                for (j=0; j<47; j++)
                                    if (strcmp(ktokenn,key_map[j][0])==0) {
                                        strncpy(xmap[xmapcnt].xkeynamel,key_map[j][1],1);
                                        strncpy(xmap[xmapcnt].xkeynameu,key_map[j][2],1);
                                    }

                                if (strcmp(ktokenl,"]")!=0)
                                    strcpy(xmap[xmapcnt].xkeyvaluel,ktokenl);
                                else
                                    strcpy(xmap[xmapcnt].xkeyvaluel,"");
                                if (strcmp(ktokenu,"]")!=0 && strcmp(ktokenu,"};\n")!=0 )
                                    strcpy(xmap[xmapcnt].xkeyvalueu,ktokenu);
                                else
                                    strcpy(xmap[xmapcnt].xkeyvalueu,"");

                                xmapcnt++;
                            }
                        }
                    }
                }
            }
        }
    }


    int ii=0,c=0;
    for (ii=0,c=0; ii<xmapcnt; ii++,c++)
    {
        char buf[7];
        memset(buf,'\0',7);
        int len=0,unidec=0;

        strncpy(map[c].keyname,xmap[ii].xkeynamel,1);
        strncpy(map[c+1].keyname,xmap[ii].xkeynameu,1);

        if (strstr(xmap[ii].xkeyvaluel,"U")!=NULL )
        {
            unidec=strtol(xmap[ii].xkeyvaluel+1, 0, 16);
            len = g_unichar_to_utf8 ((gunichar)unidec, buf);
            buf[len] = '\0';
            strcpy(map[c].keyvalue,buf);
        }
        else if (strstr(xmap[ii].xkeyvaluel,"0x1000")!=NULL )
        {
            unidec=strtol(xmap[ii].xkeyvaluel+5, 0, 16);
            len = g_unichar_to_utf8 ((gunichar)unidec, buf);
            buf[len] = '\0';
            strcpy(map[c].keyvalue,buf);
        }
        else
        {
            strcpy(map[c].keyvalue,xmap[ii].xkeyvaluel);
        }
        memset(buf,'\0',7);
        if (strstr(xmap[ii].xkeyvalueu,"U")!=NULL )
        {
            unidec=strtol(xmap[ii].xkeyvalueu+1, 0, 16);
            len = g_unichar_to_utf8 ((gunichar)unidec, buf);
            buf[len] = '\0';
            strcpy(map[c+1].keyvalue,buf);
        }
        else if (strstr(xmap[ii].xkeyvalueu,"0x1000")!=NULL )
        {
            unidec=strtol(xmap[ii].xkeyvalueu+5, 0, 16);
            len = g_unichar_to_utf8 ((gunichar)unidec, buf);
            buf[len] = '\0';
            strcpy(map[c+1].keyvalue,buf);
        }
        else
        {
            strcpy(map[c+1].keyvalue,xmap[ii].xkeyvalueu);
        }
        c++;
    }

    mapcnt=xmapcnt*2;
    gtk_toggle_button_set_active((GtkToggleButton *)l_shift_btn,FALSE);
    gtk_toggle_button_set_active((GtkToggleButton *)r_shift_btn,FALSE);
    gtk_toggle_button_set_active((GtkToggleButton *)caps_btn,FALSE);

}

char* get_iso_code(char *lname)
{
    /*Langname to keymapname*/
    xmlNode *cur_node;
    char *iso639_2tcode, *iso639_1code, *iso639_name;

    xmlDocPtr doc;
    doc = xmlParseFile("/usr/share/xml/iso-codes/iso_639.xml");

    if (doc == NULL)
        printf("error: could not parse file file.xml\n");

    xmlNode *root = NULL;
    root = xmlDocGetRootElement(doc);

    if ( !root ||
            !root->name ||
            xmlStrcmp(root->name,(const xmlChar *)"iso_639_entries") )
    {
        xmlFreeDoc(doc);
        return FALSE;
    }

    for (cur_node = root->children; cur_node != NULL; cur_node = cur_node->next)
    {
        if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(cur_node->name, (const xmlChar *) "iso_639_entry" ) )
        {
            iso639_2tcode = xmlGetProp(cur_node,(const xmlChar *)"iso_639_2T_code");
            iso639_1code = xmlGetProp(cur_node,(const xmlChar *)"iso_639_1_code");
            iso639_name = xmlGetProp(cur_node,(const xmlChar *)"name");

            if (lname && (strcmp(iso639_name,lname)==0))
            {
                if (iso639_1code) {
                    return iso639_1code;
                }
                if (!iso639_1code && iso639_2tcode) {
                    return iso639_2tcode;
                }
            }
            else
            {
                if (strstr(iso639_name,";")!= NULL)
                {
                    char *ktoken;
                    ktoken = strtok(iso639_name, ";");
                    if (ktoken == NULL)
                        break;
                    while (ktoken!= NULL)
                    {
                        if (strstr(ktoken,lname)!=NULL)
                        {
                            if (iso639_1code) {
                                return iso639_1code;
                            }
                            if (!iso639_1code && iso639_2tcode) {
                                return iso639_2tcode;
                            }
                        }
                        ktoken = strtok(NULL, ";");
                    }
                }

            }
            xmlFree(iso639_1code);
            xmlFree(iso639_2tcode);
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return "";
}
void get_mappings(char *mfile)
{


    FILE *fp,*ifp;
    char line[80],iline[80];
    char mfile_path[80],hfile_path[80];
    char *home_dir;
    const char delims[] = "\" ()";
    char *result = NULL,*res1=NULL;
    int linelen=0,tokcnt=0;
    char *ptr,local[4],mmfile[15];
    mapcnt=0;

    memset(local,'\0',4);
    memset(mmfile,'\0',15);

    if (strstr(mfile,"xkb-")!=NULL)
    {
        get_xkb_mappings(mfile);
        load_mappings();
    }
    else
    {
        strcpy(local,get_iso_code(mfile));
        sprintf(mmfile,"%s-inscript",local);

        if (local[0] == '\0')
        {
            home_dir=getenv("HOME");
            strcpy(hfile_path,home_dir);
            strcat(hfile_path,"/.m17n.d/");
            strncat(hfile_path,mfile,strlen(mfile));
            strcat(hfile_path,".mim");

            strcpy(mfile_path,"/usr/share/m17n/");
            strncat(mfile_path,mfile,strlen(mfile));
            strcat(mfile_path,".mim");
        }
        else {
            strcpy(mfile_path,"/usr/share/m17n/");
            strncat(mfile_path,mmfile,strlen(mmfile));
            strcat(mfile_path,".mim");
        }

        struct stat buf;

        if ( stat ( mfile_path, &buf ) == 0 )
            fp = fopen(mfile_path, "r");
        else if ( stat ( hfile_path, &buf ) == 0 )
            fp = fopen(hfile_path, "r");
        else
            exit(0);
        while (fgets(line,80,fp) != NULL)
        {
            linelen=strlen(line);
            line[linelen-1]='\0';

            //for parsing kn-kgp.mim
            char *pline;
            pline= (char *)malloc(80*sizeof(char));
            memset(pline,'\0',80);

            int pat=0;
            pline=strchr(line,'(');
            if (pline != NULL && strstr(pline,"\(\"V\") \(\"W\") \(\"Y\") \(\"Z\")")== NULL) {
                for (pat=0; pat<4; pat++)
                {
                    pline=strchr(pline+1,'(');
                    if (pline == NULL)
                        break;
                }
            }
            // Only parse valid lines with 1:1 keypair
            if (pline == NULL && strstr(line,"(\"")!= NULL && strstr(line,"delete")==NULL)
            {
                tokcnt=0;
                if (strstr(line,"(\"(\"")!=NULL) {
                    char delim1[]="\" )";
                    strncpy(map[mapcnt].keyname,"(",1);
                    res1=strtok( line, delim1 );
                    if (res1[0]=='?'&& strlen(res1)!=1) {
                        ptr=strchr(res1,'?');
                        *ptr++='\0';
                        strcpy(map[mapcnt].keyvalue,ptr);
                    }
                    else {
                        strcpy(map[mapcnt].keyvalue,res1);
                    }
                    mapcnt++;

                }
                if (strstr(line,"(\")\"")!=NULL) {
                    char delim1[]="\" (";
                    strncpy(map[mapcnt].keyname,")",1);
                    res1=strtok( line, delim1 );
                    if (res1[0]=='?'&& strlen(res1)!=1) {
                        ptr=strchr(res1,'?');
                        *ptr++='\0';
                        strcpy(map[mapcnt].keyvalue,ptr);
                    }
                    else {
                        strcpy(map[mapcnt].keyvalue,res1);
                    }
                    mapcnt++;
                }

                if (strstr(line,"(\"\\\"")!=NULL) {
                    char delim1[]="\\\" ()";
                    strncpy(map[mapcnt].keyname,"\"",1);
                    res1=strtok( line, delim1 );
                    if (res1 == NULL)
                    {
                        strcpy(map[mapcnt].keyvalue,"\"");
                        mapcnt++;
                    }
                    else if (res1[0]=='?'&& strlen(res1)!=1) {
                        ptr=strchr(res1,'?');
                        *ptr++='\0';
                        strcpy(map[mapcnt].keyvalue,ptr);
                    }
                    else {
                        strcpy(map[mapcnt].keyvalue,res1);
                    }
                    mapcnt++;
                }

                if (strstr(line,"(\"\\\\\"")!=NULL) {
                    char delim1[]="\"\\ ()";
                    strncpy(map[mapcnt].keyname,"\\",1);
                    res1=strtok( line, delim1 );

                    if (res1==NULL) {
                        strcpy(map[mapcnt].keyvalue,"\\");
                    } else if (res1[0]=='?'&& strlen(res1)!=1) {
                        ptr=strchr(res1,'?');
                        *ptr++='\0';
                        strcpy(map[mapcnt].keyvalue,ptr);
                    }
                    else if (res1[0]=='?'&& strlen(res1)==1) {
                        strcpy(map[mapcnt].keyvalue,"\\");
                    } else {
                        strcpy(map[mapcnt].keyvalue,res1);
                    }
                    mapcnt++;

                }

                result = strtok( line, delims );
                while ( result != NULL && tokcnt<2 ) {
                    if (strstr(result," (")!=NULL || strstr(result,"\\")!=NULL)
                        result = strtok( NULL, delims );
                    else {
                        if (tokcnt == 0) {
                            if (strlen(result)>1)
                                tokcnt=2;
                            else
                                strncpy(map[mapcnt].keyname,result,1);
                        }
                        if (tokcnt == 1) {
                            if (result[0]=='?'&& strlen(result)!=1) {
                                ptr=strchr(result,'?');
                                *ptr++='\0';
                                strcpy(map[mapcnt].keyvalue,ptr);
                            }
                            else if (result[0]=='\\') {
                                ptr=strchr(result,'?');
                                *ptr++='\0';
                                strcpy(map[mapcnt].keyvalue,ptr);
                            }
                            else {
                                strcpy(map[mapcnt].keyvalue,result);
                            }
                            mapcnt++;
                        }

                        if (tokcnt == 2 && strlen(result)>2) {
                            GtkWidget *mdialog;

                            mdialog = gtk_message_dialog_new (GTK_WINDOW(IndicMapper),
                                                              GTK_DIALOG_DESTROY_WITH_PARENT,
                                                              GTK_MESSAGE_ERROR,
                                                              GTK_BUTTONS_CLOSE,
                                                              NULL);
                            gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (mdialog),
                                                           "This keymap file does not map uniquely to a keyboard and therefore cannot be displayed correctly");
                            gtk_dialog_run (GTK_DIALOG (mdialog));
                            gtk_widget_destroy (mdialog);
                            gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,0);
                            english_mappings();
                            goto NotValidKeymap;
                        }
                        tokcnt++;
                    }
                    result = strtok( NULL, delims );
                }
            }
        }
        gtk_toggle_button_set_active((GtkToggleButton *)l_shift_btn,FALSE);
        gtk_toggle_button_set_active((GtkToggleButton *)r_shift_btn,FALSE);
        gtk_toggle_button_set_active((GtkToggleButton *)caps_btn,FALSE);

        load_mappings();
NotValidKeymap:
        fclose(fp);
    }
}
void print_xkb_map()
{
    int i=0;
    for (i=0; i< 47; i++)
    {
        printf("    Key <%s> { [",key_map[i][0]);
        int n1,c1,len1;
        wchar_t *wp1,*wp2,*wp3,*wp4;
        char p1[MB_CUR_MAX+1],p2[MB_CUR_MAX+1],p3[MB_CUR_MAX+1],p4[MB_CUR_MAX+1];
        char unichar1[5],unichar2[5],unichar3[5],unichar4[5];
        char *entry_text1,*entry_text2,*entry_text3,*entry_text4;

        entry_text1=srch(key_map[i][1]);
        n1 = strlen(entry_text1) * sizeof(wchar_t);
        wp1 = (wchar_t *)malloc(n1);
        len1 = mbstowcs(wp1, entry_text1, n1);
        if (strcmp(entry_text1,"ZWNJ")!=0 && strcmp(entry_text1,"ZWJ")!=0)
        {
            for (c1=0; c1<len1; c1++) {
                int x1;
                x1 = wctomb(p1, wp1[c1]);
                if (x1>0) p1[x1]=0;
                sprintf(unichar1, "%04X", wp1[c1]);

                printf(" U%04X",strtol(unichar1, 0, 16));
            }
        }
        entry_text2=srch(key_map[i][2]);

        n1 = strlen(entry_text2) * sizeof(wchar_t);
        wp2 = (wchar_t *)malloc(n1);
        len1 = mbstowcs(wp2, entry_text2, n1);
        if (strcmp(entry_text2,"ZWNJ")!=0 && strcmp(entry_text2,"ZWJ")!=0)
        {
            for (c1=0; c1<len1; c1++) {
                int x1;
                x1 = wctomb(p2, wp2[c1]);
                if (x1>0) p2[x1]=0;
                sprintf(unichar2, "%04X", wp2[c1]);
                printf(", U%04X",strtol(unichar2, 0, 16));
            }
        }
        printf("] };\n");
    }
}

void set_default_map_name(char *lcode)
{
    gint i=0;
    char filetopen[256];
    FILE *mfp;
    char mline[80],*mimname,*token;
    char *mlinecpy;

    strcpy(filetopen,"/usr/share/m17n/");
    strcat(filetopen,lcode);
    strcat(filetopen,"-inscript.mim");
    mfp = fopen(filetopen, "r");

    if ( mfp != NULL )
    {
        if (fgets(mline,80,mfp) != NULL)
        {
            mlinecpy=(char *)malloc(80*sizeof(char));
            langname=(char *)malloc(20*sizeof(char));
            memset(mlinecpy,'\0',80);
            memset(langname,'\0',20);

            strcpy(mlinecpy,mline);
            token = strtok(mline, "-; ");

            if (strlen(token)==2)
            {
                int ccnt=23;
                while (mlinecpy[ccnt++]!=' ');

                strncpy(langname,mlinecpy+22,ccnt-23);
                langname[ccnt-23]='\0';
            }
            if (strlen(token)==3)
            {
                int ccnt=24;
                while (mlinecpy[ccnt++]!=' ');

                strncpy(langname,mlinecpy+23,ccnt-24);
                langname[ccnt-24]='\0';
            }
        }

        for ( ii=0; ii<mimcnt; ii++)
        {
            if (strcmp(maplist[ii],langname)==0)
            {
                gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,ii+1);
                break;
            }
        }
    }
    else
    {
        english_mappings();
        /* This mean iok is running in non-Indic and non-English locale */
        gtk_widget_show (combo_mim);
    }
}

void add_xkbmaplist()
{
    FILE *ifp;
    int i=0,j=0;
    char iline[80];
    char xkfile[65];
    char *ntoken;

    ifp= fopen("/usr/share/X11/xkb/symbols/in", "r");
    while (fgets(iline,80,ifp) != NULL)
    {
        if (strstr(iline,"xkb_symbols")!=NULL)
        {
            ntoken=(char *) malloc(sizeof(char) * 25);
            memset(ntoken,'\0',25);
            ntoken = strtok(iline, " ");
            ntoken = strtok(NULL, " \"");
        }
        if (strstr(iline,"name[Group1]")!=NULL)
        {
            if (strcmp(iline,"\tname[Group1]=\"India\";\n")==0)
            {
                sprintf(xkfile,"xkb-%s", "India");
                gtk_combo_box_append_text ((GtkComboBox *)combo_mim, xkfile);
                maplist[mimcnt]= (char *) malloc(strlen(xkfile)+1);
                strcpy(maplist[mimcnt++],xkfile);
            }
            if (strcmp(iline,"\tname[Group1]=\"India\";\n")!=0)
            {
                char *itoken,*nntoken;
                size_t nn=0,nn_cnt=0;
                itoken=(char *)malloc(65*sizeof(char));
                memset(itoken,'\0',65);

                nn=strlen(iline);
                while (nn_cnt < nn)
                {
                    if (iline[nn_cnt] == '\"')
                    {
                        strncpy(itoken,iline+nn_cnt+1,nn-nn_cnt-4);
                        break;
                    }
                    else
                        nn_cnt++;
                }
                sprintf(xkfile,"xkb-%s", itoken);
                if (strstr(xkfile,"typewriter")==NULL && strstr(xkfile, "Urdu")==NULL && strstr(xkfile, "English")==NULL)
                {
                    gtk_combo_box_append_text ((GtkComboBox *)combo_mim, xkfile);
                    maplist[mimcnt]= (char *) malloc(strlen(xkfile)+1);
                    strcpy(maplist[mimcnt++],xkfile);
                }
            }
        }
    }
}
void add_mimfilelist()
{
    char* std_dir_path;
    char* home_dir;
    char custom_dir_path[256];
    char mimfile[PATH_MAX+1];
    int mimlen=0;
    struct dirent **namelist;
    int n,cnt=0;

    std_dir_path ="/usr/share/m17n/";

    gtk_combo_box_insert_text ((GtkComboBox *)combo_mim,0, "English");
    gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,0);

    n = scandir(std_dir_path, &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        while (cnt<n)
        {
            mimlen=strlen(namelist[cnt]->d_name);
            if ((mimlen>=6) && (strstr(namelist[cnt]->d_name,".mim")!=NULL) && (namelist[cnt]->d_name[mimlen-1]=='m') && (strstr(namelist[cnt]->d_name,"inscript.mim")!=NULL))
            {
                char mline[80],*token,fpath[256];
                FILE *mfp;
                sprintf(fpath,"%s%s",std_dir_path,namelist[cnt]->d_name);
                mfp = fopen(fpath, "r");

                if (fgets(mline,80,mfp) != NULL)
                {
                    mlinecpy=(char *)malloc(80*sizeof(char));
                    langname=(char *)malloc(20*sizeof(char));

                    strcpy(mlinecpy,mline);
                    token = strtok(mline, "-; ");

                    if (strlen(token)==2)
                    {
                        int ccnt=23;
                        while (mlinecpy[ccnt++]!=' ');

                        strncpy(langname,mlinecpy+22,ccnt-23);
                        langname[ccnt-23]='\0';
                    }
                    if (strlen(token)==3)
                    {
                        int ccnt=24;
                        while (mlinecpy[ccnt++]!=' ');

                        strncpy(langname,mlinecpy+23,ccnt-24);
                        langname[ccnt-24]='\0';
                    }

                    maplist[mimcnt]= (char *) malloc(strlen(langname)+1);
                    strcpy(maplist[mimcnt],langname);
                    gtk_combo_box_append_text ((GtkComboBox *)combo_mim, langname);
                    mimcnt++;
                }
            }
            free(namelist[cnt++]);
        }
        free(namelist);
    }

    cnt=0;
    home_dir=getenv("HOME");
    strcpy(custom_dir_path,home_dir);
    strcat(custom_dir_path,"/.m17n.d/");

    n = scandir(custom_dir_path, &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        while (cnt<n)
        {
            mimlen=strlen(namelist[cnt]->d_name);
            if ((mimlen>=6) && (strstr(namelist[cnt]->d_name,".mim")!=NULL) && (namelist[cnt]->d_name[mimlen-1]=='m') && (strstr(namelist[cnt]->d_name,"inscript.mim")!=NULL))
            {
                strncpy(mimfile,namelist[cnt]->d_name,mimlen-4);
                mimfile[mimlen-4]='\0';
                maplist[mimcnt]= (char *) malloc(strlen(mimfile)+1);
                strcpy(maplist[mimcnt],mimfile);
                gtk_combo_box_append_text ((GtkComboBox *)combo_mim, mimfile);
                mimcnt++;
            }
            free(namelist[cnt++]);
        }
        free(namelist);
    }
}

void clear_mimfilelist()
{

    for ( ii=0; ii<mimcnt; ii++)
        gtk_combo_box_remove_text((GtkComboBox *)combo_mim,ii);
    add_mimfilelist();
}

static void on_combo_mim_changed( GtkWidget *widget,
                                  GtkWidget *combo_mim )
{
    gchar *current_mim;

    current_mim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);
    clear_mappings();

    if (strcmp(current_mim,"English")!=0)
        get_mappings(current_mim);
    else
        english_mappings();
}

void on_l_shift_btn_clicked( GtkWidget *widget,
                             gpointer   data )
{
    gchar *current_mim;

    current_mim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn))
    {
        if (strcmp(current_mim,"English")!=0 )
            load_shift_mappings(1);
        else
            english_shift_mappings(1);
    }
    else
    {
        if (!gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn) )
        {
            if (strcmp(current_mim,"English")!=0 )
                load_mappings();
            else
                english_mappings();
        }
        if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn))
        {
            if (strcmp(current_mim,"English")!=0 )
                load_shift_mappings(0);
            else
                english_shift_mappings(0);
        }
    }
}

void on_r_shift_btn_clicked( GtkWidget *widget,
                             gpointer   data )
{
    gchar *current_mim;

    current_mim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);
    if (gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
    {
        if (strcmp(current_mim,"English")!=0 )
            load_shift_mappings(1);
        else
            english_shift_mappings(1);
    }
    else
    {
        if (!gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn))
        {
            if (strcmp(current_mim,"English")!=0 )
                load_mappings();
            else
                english_mappings();
        }
        if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn))
        {
            if (strcmp(current_mim,"English")!=0 )
                load_shift_mappings(0);
            else
                english_shift_mappings(0);
        }
    }
}

void check_shift_btn()
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn))
    {
        gtk_toggle_button_set_active((GtkToggleButton *)l_shift_btn,FALSE);
        on_l_shift_btn_clicked(l_shift_btn,NULL);
    }
    if (gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
    {
        gtk_toggle_button_set_active((GtkToggleButton *)r_shift_btn,FALSE);
        on_r_shift_btn_clicked(r_shift_btn,NULL);
    }
}

static void on_caps_btn_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    gchar *current_mim;

    current_mim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);
// clear_mappings();
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn))
    {
        if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn)|| gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        {
            if (strcmp(current_mim,"English")!=0 )
                load_shift_mappings(1);
            else
                english_shift_mappings(1);
        }
        else
        {
            if (strcmp(current_mim,"English")!=0 )
                load_shift_mappings(0);
            else
                english_shift_mappings(0);
        }
    }
    else
    {
        if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn)|| gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        {
            if (strcmp(current_mim,"English")!=0 )
                load_shift_mappings(1);
            else
                english_shift_mappings(1);
        }
        else
        {
            if (strcmp(current_mim,"English")!=0 )
                load_mappings();
            else
                english_mappings();
        }
    }
}

char * display_key_value(char *key)
{
    int c, n, len;
    wchar_t *wp;
    char p[MB_CUR_MAX+1];
    char unichar[5];
    char joiner[5];
    char *entry_text;

    entry_text=srch(key);
    if (strcmp(entry_text,"ZWNJ")==0)
    {
        strcpy(joiner,"‌");
        n = strlen(joiner) * sizeof(wchar_t);
        wp = (wchar_t *)malloc(n);
        len = mbstowcs(wp, joiner, n);
    }
    else if (strcmp(entry_text,"ZWJ")==0)
    {
        strcpy(joiner,"‍");
        n = strlen(joiner) * sizeof(wchar_t);
        wp = (wchar_t *)malloc(n);
        len = mbstowcs(wp, joiner, n);
    }
    else
    {
        n = strlen(entry_text) * sizeof(wchar_t);
        wp = (wchar_t *)malloc(n);
        len = mbstowcs(wp, entry_text, n);

    }

    for (c=0; c<len; c++) {
        int x;
        x = wctomb(p, wp[c]);
        if (x>0) p[x]=0;
        sprintf(unichar, "%04X", wp[c]);

        key_event_send_char(strtol(unichar, 0, 16));
    }
    check_shift_btn();
    return srch(key);

}

static void on_bt_back_quote_clicked( GtkWidget *widget,
                                      gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("~");
    else
        display_key_value("`");
}
static void on_bt_1_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("!");
    else
        display_key_value("1");
}
static void on_bt_2_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("@");
    else
        display_key_value("2");
}
static void on_bt_3_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("#");
    else
        display_key_value("3");
}
static void on_bt_4_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("$");
    else
        display_key_value("4");
}
static void on_bt_5_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("%");
    else
        display_key_value("5");
}
static void on_bt_6_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("^");
    else
        display_key_value("6");
}
static void on_bt_7_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("&");
    else
        display_key_value("7");
}
static void on_bt_8_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("*");
    else
        display_key_value("8");
}
static void on_bt_9_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("(");
    else
        display_key_value("9");
}
static void on_bt_0_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value(")");
    else
        display_key_value("0");
}
static void on_bt_dash_clicked( GtkWidget *widget,
                                gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("_");
    else
        display_key_value("-");
}
static void on_bt_equal_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("+");
    else
        display_key_value("=");
}

static void on_bt_q_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("Q");
    else
        display_key_value("q");
}

static void on_bt_w_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("W");
    else
        display_key_value("w");
}

static void on_bt_e_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("E");
    else
        display_key_value("e");
}

static void on_bt_r_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("R");
    else
        display_key_value("r");
}
static void on_bt_t_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("T");
    else
        display_key_value("t");
}
static void on_bt_y_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("Y");
    else
        display_key_value("y");
}
static void on_bt_u_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("U");
    else
        display_key_value("u");
}
static void on_bt_i_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("I");
    else
        display_key_value("i");
}
static void on_bt_o_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("O");
    else
        display_key_value("o");
}
static void on_bt_p_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("P");
    else
        display_key_value("p");
}
static void on_bt_sq_bra_left_clicked( GtkWidget *widget,
                                       gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("{");
    else
        display_key_value("[");
}

static void on_bt_sq_bra_right_clicked( GtkWidget *widget,
                                        gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("}");
    else
        display_key_value("]");
}

static void on_bt_win_slash_clicked( GtkWidget *widget,
                                     gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("|");
    else
        display_key_value("\\");
}

static void on_bt_a_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("A");
    else
        display_key_value("a");
}
static void on_bt_s_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("S");
    else
        display_key_value("s");
}
static void on_bt_d_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("D");
    else
        display_key_value("d");
}
static void on_bt_f_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("F");
    else
        display_key_value("f");
}
static void on_bt_g_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("G");
    else
        display_key_value("g");
}
static void on_bt_h_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("H");
    else
        display_key_value("h");
}
static void on_bt_j_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("J");
    else
        display_key_value("j");
}
static void on_bt_k_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("K");
    else
        display_key_value("k");
}
static void on_bt_l_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("L");
    else
        display_key_value("l");
}
static void on_bt_semi_colon_clicked( GtkWidget *widget,
                                      gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value(":");
    else
        display_key_value(";");
}
static void on_bt_quote_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("\"");
    else
        display_key_value("'");
}
static void on_bt_z_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("Z");
    else
        display_key_value("z");
}
static void on_bt_x_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("X");
    else
        display_key_value("x");
}
static void on_bt_c_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("C");
    else
        display_key_value("c");
}
static void on_bt_v_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("V");
    else
        display_key_value("v");
}
static void on_bt_b_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("B");
    else
        display_key_value("b");
}
static void on_bt_n_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("N");
    else
        display_key_value("n");
}
static void on_bt_m_clicked( GtkWidget *widget,
                             gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("M");
    else
        display_key_value("m");
}
static void on_bt_coma_clicked( GtkWidget *widget,
                                gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("<");
    else
        display_key_value(",");
}
static void on_bt_dot_clicked( GtkWidget *widget,
                               gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value(">");
    else
        display_key_value(".");
}
static void on_bt_linux_slash_clicked( GtkWidget *widget,
                                       gpointer   data )
{
    if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
        display_key_value("?");
    else
        display_key_value("/");
}

static void on_enter_btn_clicked( GtkWidget *widget,
                                  gpointer   data )
{
    key_event_send_enter();
}

static void on_backspace_btn_clicked( GtkWidget *widget,
                                      gpointer   data )
{
    key_event_send_backspace();
}

static void on_tab_btn_clicked( GtkWidget *widget,
                                gpointer   data )
{
    key_event_send_tab();
}

static void on_esc_btn_clicked( GtkWidget *widget,
                                gpointer   data )
{
    key_event_send_escape();
}

static void on_bt_f1_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f1();
}
static void on_bt_f2_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f2();
}
static void on_bt_f3_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f3();
}
static void on_bt_f4_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f4();
}
static void on_bt_f5_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f5();
}
static void on_bt_f6_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f6();
}
static void on_bt_f7_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f7();
}
static void on_bt_f8_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f8();
}
static void on_bt_f9_clicked( GtkWidget *widget,
                              gpointer   data )
{
    key_event_send_f9();
}
static void on_bt_f10_clicked( GtkWidget *widget,
                               gpointer   data )
{
    key_event_send_f10();
}
static void on_bt_f11_clicked( GtkWidget *widget,
                               gpointer   data )
{
    key_event_send_f11();
}
static void on_bt_f12_clicked( GtkWidget *widget,
                               gpointer   data )
{
    key_event_send_f12();
}

static void on_keyswitch_btn_clicked( GtkWidget *widget,
                                      gpointer   data )
{
    gchar *current_mim,*mimname;
    gint i=0;
    gchar tt_msg[255];

    current_mim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);
    strcpy(toggle_keymap,"to ");
    strcat(toggle_keymap,current_mim);
    if (strcmp(current_mim,"English")!=0 && strcmp(gtk_button_get_label((GtkButton *)keyswitch_btn),"to English")==0)
    {
        clear_mappings();
        gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,0);

        if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
            english_shift_mappings(1);
        else
        {
            if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn))
                english_shift_mappings(0);
        }
        gtk_button_set_label((GtkButton *)keyswitch_btn,toggle_keymap);
        gtk_widget_set_sensitive((GtkWidget *)combo_mim,FALSE );
        tooltips = gtk_tooltips_new ();
        memset(tt_msg,'\0',255);
        sprintf(tt_msg,"Click on Toggle button labeled %s to activate this",toggle_keymap);
        gtk_tooltips_set_tip (tooltips, combo_mim, _(tt_msg), NULL);
        tooltips = gtk_tooltips_new ();
        memset(tt_msg,'\0',255);
        sprintf(tt_msg,"Switch %s keymap",toggle_keymap);
        gtk_tooltips_set_tip (tooltips, (GtkWidget *)keyswitch_btn, _(tt_msg), NULL);

    }

    if (strcmp(current_mim,"English")==0 && strcmp(gtk_button_get_label((GtkButton *)keyswitch_btn),"to English")!=0)
    {
        clear_mappings();
        strcpy(map_to_read,(char *)gtk_button_get_label((GtkButton *)keyswitch_btn));
        get_mappings(map_to_read+3);
        gtk_widget_set_sensitive((GtkWidget *)combo_mim, TRUE);
        tooltips = gtk_tooltips_new ();
        gtk_tooltips_set_tip (tooltips, combo_mim, _("Select the language in which you want to write"), NULL);
        tooltips = gtk_tooltips_new ();
        gtk_tooltips_set_tip (tooltips, keyswitch_btn, _("Change to English Keyboard"), NULL);

        for ( ii=0; ii<mimcnt; ii++)
        {
            if (strcmp(maplist[ii],map_to_read+3)==0)
            {
                gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,ii+1);
                break;
            }
        }

        gtk_button_set_label((GtkButton *)keyswitch_btn,"to English");
        if (gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn))
            load_shift_mappings(1);
        else
        {
            if (!gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn) )
                load_mappings();
            if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn))
                load_shift_mappings(0);
        }

    }

}

static void on_space_btn_clicked( GtkWidget *widget,
                                  gpointer   data )
{
    key_event_send_space();
}

static void on_bt_about_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    GtkWidget *about_IndicMapper;
    about_IndicMapper = create_IndicMapper_aboutdialog ();
    gtk_widget_show (about_IndicMapper);

}
static void on_btn_reassign_reakey_clicked( GtkWidget *widget,
        gpointer   data )
{
    gtxtkeya=(char *)gtk_entry_get_text(GTK_ENTRY(reakeya_entry));
    gtxtkeyb=(char *)gtk_entry_get_text(GTK_ENTRY(reakeyb_entry));
    gtxtvaluea=srch(gtxtkeya);
    gtxtvalueb=srch(gtxtkeyb);

    if (strlen(gtxtvaluea)>=1 && strlen(gtxtvalueb)>=1 )
    {
        for (ii=0; ii<mapcnt; ii++) {
            if (strcmp(map[ii].keyname,gtxtkeyb)==0)
                strcpy(map[ii].keyvalue,gtxtvaluea);
        }
        load_mappings(mapcnt);
    }
    gtk_widget_hide(Reassign_Key);

}


static void on_btn_cancel_reakey_clicked( GtkWidget *widget,
        gpointer   data )
{
    gtk_widget_hide(Reassign_Key);
}


static void on_bt_reakey_clicked( GtkWidget *widget,
                                  gpointer   data )
{

    Reassign_Key = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (Reassign_Key), _("ReassignKey"));

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (Reassign_Key), vbox1);

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox2);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

    lbl_rk1 = gtk_label_new (_("Reassign Keyvalue of Key"));
    gtk_widget_show (lbl_rk1);
    gtk_box_pack_start (GTK_BOX (hbox2), lbl_rk1, FALSE, FALSE, 0);

    reakeya_entry = gtk_entry_new ();
    gtk_widget_show (reakeya_entry);
    gtk_box_pack_start (GTK_BOX (hbox2), reakeya_entry, TRUE, TRUE, 0);
    gtk_entry_set_max_length (GTK_ENTRY (reakeya_entry), 1);
    gtk_entry_set_invisible_char (GTK_ENTRY (reakeya_entry), 8226);
    gtk_entry_set_width_chars (GTK_ENTRY (reakeya_entry), 5);

    lbl_rk2 = gtk_label_new (_("to another Key"));
    gtk_widget_show (lbl_rk2);
    gtk_box_pack_start (GTK_BOX (hbox2), lbl_rk2, FALSE, FALSE, 0);

    reakeyb_entry = gtk_entry_new ();
    gtk_widget_show (reakeyb_entry);
    gtk_box_pack_start (GTK_BOX (hbox2), reakeyb_entry, TRUE, TRUE, 0);
    gtk_entry_set_max_length (GTK_ENTRY (reakeyb_entry), 1);
    gtk_entry_set_invisible_char (GTK_ENTRY (reakeyb_entry), 8226);
    gtk_entry_set_width_chars (GTK_ENTRY (reakeyb_entry), 5);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

    btn_reassign_reakey = gtk_button_new_with_mnemonic (_("Reassign"));
    gtk_widget_show (btn_reassign_reakey);
    gtk_box_pack_start (GTK_BOX (hbox1), btn_reassign_reakey, FALSE, FALSE, 0);

    btn_cancel_reakey = gtk_button_new_with_mnemonic (_("Cancel"));
    gtk_widget_show (btn_cancel_reakey);
    gtk_box_pack_start (GTK_BOX (hbox1), btn_cancel_reakey, FALSE, FALSE, 0);


    g_signal_connect (G_OBJECT (btn_cancel_reakey), "clicked",
                      G_CALLBACK (on_btn_cancel_reakey_clicked),
                      (gpointer) btn_cancel_reakey);

    g_signal_connect (G_OBJECT (btn_reassign_reakey), "clicked",
                      G_CALLBACK (on_btn_reassign_reakey_clicked),
                      (gpointer) btn_reassign_reakey);

    gtk_widget_show(Reassign_Key);

}

void
on_entry1_activate                     (GtkEntry        *entry,
                                        gpointer         user_data)
{
    gtk_widget_grab_focus(keyb_entry);
}


static void on_bt_swapkey_clicked( GtkWidget *widget,
                                   gpointer   data )
{

    swap_dialog = gtk_dialog_new_with_buttons("Swap Keys", NULL, 0,
                  GTK_STOCK_CANCEL,
                  GTK_RESPONSE_CANCEL,
                  GTK_STOCK_OK,
                  GTK_RESPONSE_OK,
                  NULL);
    gtk_dialog_set_has_separator(GTK_DIALOG(swap_dialog), FALSE);
    gtk_dialog_set_default_response(GTK_DIALOG(swap_dialog), GTK_RESPONSE_OK);

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(swap_dialog)->vbox), hbox2);

    keya = gtk_label_new ("English Key 1");
    gtk_box_pack_start (GTK_BOX (hbox2), keya, TRUE, TRUE, 0);

    keya_entry = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (hbox2), keya_entry, TRUE, TRUE, 0);
    gtk_entry_set_max_length (GTK_ENTRY (keya_entry), 1);

    hbox3 = gtk_hbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(swap_dialog)->vbox), hbox3);

    keyb = gtk_label_new ("English Key 2");
    gtk_box_pack_start (GTK_BOX (hbox3), keyb, TRUE, TRUE, 0);

    keyb_entry = gtk_entry_new ();
    gtk_entry_set_activates_default(GTK_ENTRY(keyb_entry), TRUE);
    gtk_box_pack_start (GTK_BOX (hbox3), keyb_entry, TRUE, TRUE, 0);
    gtk_entry_set_max_length (GTK_ENTRY (keyb_entry), 1);

    g_signal_connect ((gpointer) keya_entry, "activate",
                      G_CALLBACK (on_entry1_activate),
                      NULL);

    gtk_widget_show_all(swap_dialog);

    gint result = gtk_dialog_run (GTK_DIALOG (swap_dialog));
    switch (result)
    {
    case GTK_RESPONSE_OK:
        gtxtkeya=(char *)gtk_entry_get_text(GTK_ENTRY(keya_entry));
        gtxtkeyb=(char *)gtk_entry_get_text(GTK_ENTRY(keyb_entry));
        gtxtvaluea=srch(gtxtkeya);
        gtxtvalueb=srch(gtxtkeyb);

        tmpkey=(char *) malloc((sizeof(char) * strlen(gtxtvaluea))+1);
        strcpy(tmpkey,gtxtvaluea);
        for (ii=0; ii<mapcnt; ii++) {
            if (strcmp(map[ii].keyname,gtxtkeya)==0)
                strcpy(map[ii].keyvalue,gtxtvalueb);
            if (strcmp(map[ii].keyname,gtxtkeyb)==0)
                strcpy(map[ii].keyvalue,tmpkey);
        }
        load_mappings(mapcnt);
        break;
    default:
        break;
    }
    gtk_widget_destroy(swap_dialog);
}

static void on_btn_writemap_clicked(gchar *filenameprefix)
{
    char *current_mim,*lc,imname[256];
    FILE *fpm;
    char *home_dir;
    char nfilepath[80],hdirpath[80];
    struct stat buf;
    int ret=0;

    home_dir=getenv("HOME");

    current_mim = (char *)gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);

    lc=get_iso_code(current_mim);
    sprintf(imname,"(input-method %s %s-inscript)\n\n",lc,filenameprefix);
    strcpy(hdirpath,home_dir);
    strcat(hdirpath,"/.m17n.d/");

    if ( stat (hdirpath , &buf ) != 0 )
    {
        printf("directory %s does not exists",hdirpath);
        ret=mkdir(hdirpath,0755);
        if (ret!=0)
            exit(0);

    }

    sprintf(nfilepath,"%s/.m17n.d/%s-%s-inscript.mim",home_dir,lc,filenameprefix);
    nfilepath[strlen(nfilepath)]='\0';
    fpm = fopen(nfilepath, "w+");

    fputs(imname,fpm);
    fputs("(description \"This is custom keyboard layout created by iok application.\")\n\n",fpm);
    if (strstr(current_mim,"as")!=NULL)
        fputs("(title \"ক\")\n",fpm);
    if (strstr(current_mim,"bn")!=NULL)
        fputs("(title \"ক\")\n",fpm);
    if (strstr(current_mim,"gu")!=NULL)
        fputs("(title \"ક\")\n",fpm);
    if (strstr(current_mim,"hi")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"mai")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"mr")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"kn")!=NULL)
        fputs("(title \"ಕ\")\n",fpm);
    if (strstr(current_mim,"ks@devanagari")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"kok")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"ml")!=NULL)
        fputs("(title \"ക\")\n",fpm);
    if (strstr(current_mim,"or")!=NULL)
        fputs("(title \"କ\")\n",fpm);
    if (strstr(current_mim,"pa")!=NULL)
        fputs("(title \"ਕ\")\n",fpm);
    if (strstr(current_mim,"sat")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"sd@devanagari")!=NULL)
        fputs("(title \"क\")\n",fpm);
    if (strstr(current_mim,"ta")!=NULL)
        fputs("(title \"க\")\n",fpm);
    if (strstr(current_mim,"te")!=NULL)
        fputs("(title \"క\")\n",fpm);

    fputs("(map\n",fpm);
    fputs(" (trans\n",fpm);
    fputs("\n",fpm);

    for (ii=0; ii<mapcnt; ii++) {
        if (strcmp(map[ii].keyname,"\"")==0 && strcmp((gchar *)map[ii].keyvalue,"\"")!=0) {
            sprintf(line, "  (\"\\%s\" \"%s\")\n", map[ii].keyname,map[ii].keyvalue);
        }
        else if (strcmp(map[ii].keyname,"\"")==0 && strcmp((gchar *)map[ii].keyvalue,"\"")==0) {
            sprintf(line, "  (\"\\%s\" \"\\%s\")\n", map[ii].keyname,map[ii].keyvalue);
        }
        else if (strcmp(map[ii].keyname,"\\")==0 && strcmp((gchar *)map[ii].keyvalue,"\\")!=0 ) {
            sprintf(line, "  (\"\\%s\" \"%s\")\n", map[ii].keyname,map[ii].keyvalue);
        }
        else if (strcmp(map[ii].keyname,"\\")==0 && strcmp((gchar *)map[ii].keyvalue,"\\")==0 ) {
            sprintf(line, "  (\"\\%s\" \"\\%s\")\n", map[ii].keyname,map[ii].keyvalue);
        }
        else {
            sprintf(line, "  (\"%s\" \"%s\")\n", map[ii].keyname,map[ii].keyvalue);
        }
        fputs(line,fpm);
    }
    fputs(" ))\n",fpm);
    fputs("\n",fpm);
    fputs("(state\n",fpm);
    fputs("  (init\n",fpm);
    fputs("    (trans)))\n",fpm);
    fclose(fpm);
    gtk_widget_hide(Write_Map);

}

void open_file(char *mapname)
{
    int maplen=0, FOUND=0;
    char omimfile[15];
    char *result=NULL;
    gchar *cmim;

    result = strtok(mapname,"/");
    while ( strcmp(result,"m17n")!=0) {
        result = strtok( NULL, "/" );
    }

    result = strtok( NULL, "/" );
    maplen=strlen(result);
    strncpy(omimfile,result,maplen-4);
    omimfile[maplen-4]='\0';

    cmim= gtk_combo_box_get_active_text((GtkComboBox *)combo_mim);

    if (strcmp(cmim,"English")==0 && strcmp(gtk_button_get_label((GtkButton *)keyswitch_btn),"to English")!=0)
        on_keyswitch_btn_clicked((gpointer) keyswitch_btn, NULL);

    for ( ii=0; ii<mimcnt; ii++)
    {
        if (strcmp(maplist[ii],omimfile)==0)
        {
            gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,ii+1);
            FOUND=1;
            break;
        }
    }

    if (FOUND==0)
    {
        gtk_combo_box_append_text ((GtkComboBox *)combo_mim, omimfile);
        gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,mimcnt+1);
        maplist[mimcnt]= (char *) malloc(strlen(omimfile)+1);
        strcpy(maplist[mimcnt++],omimfile);
    }
}

static void on_bt_opmap_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new (_("Open Keymap File"),
                                          GTK_WINDOW(IndicMapper),
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);
    gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER( dialog ) ,"/usr/share/m17n");
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "m17n Keymap file (*.mim)");
    gtk_file_filter_add_pattern(filter, "*.mim");
    g_object_set_data( G_OBJECT( filter ), "id", GINT_TO_POINTER( 0 ) );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gtk_widget_destroy (dialog);

        open_file (filename);
        g_free (filename);
    }
    else
    {
        gtk_widget_destroy (dialog);
    }
}

static void on_bt_wrmap_clicked( GtkWidget *widget,
                                 gpointer   data )
{
    GtkWidget *dialog;
    char *home_dir;
    char sdirpath[80];
    struct stat buf;
    int ret=0;

    home_dir=getenv("HOME");
    strcpy(sdirpath,home_dir);
    strcat(sdirpath,"/.m17n.d/");

    if ( stat (sdirpath , &buf ) != 0 )
    {
        printf("directory %s does not exists",sdirpath);
        ret=mkdir(sdirpath,0755);
        if (ret!=0)
            exit(0);
    }

    dialog = gtk_file_chooser_dialog_new (_("Write to Keymap File"),
                                          GTK_WINDOW(IndicMapper),
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                          NULL);

    gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER( dialog ) ,sdirpath);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "m17n Keymap file (*.mim)");
    gtk_file_filter_add_pattern(filter, "*.mim");
    g_object_set_data( G_OBJECT( filter ), "id", GINT_TO_POINTER( 0 ) );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gtk_widget_destroy (dialog);

        on_btn_writemap_clicked (basename(filename));
        g_free (filename);
    }
    else
    {
        gtk_widget_destroy (dialog);
    }

}


void
on_bt_back_quote_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_back_quote_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label((GtkButton *)swidget,ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"~")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"`")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_back_quote_drag_drop  (GtkWidget       *widget,
                             GdkDragContext  *context,
                             gint             x,
                             gint             y,
                             guint            time,
                             gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_1_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_1_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label((GtkButton *)swidget,ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"!")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"1")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_1_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_2_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_2_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label((GtkButton *)swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp((char *)map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"@")==0)
                    {
                        strcpy((char *)map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"2")==0)
                {
                    strcpy((char *)map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    wcscpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_2_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_3_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_3_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp((char *)map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"#")==0)
                    {
                        strcpy((char *)map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"3")==0)
                {
                    strcpy((char *)map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    wcscpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_3_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_4_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_4_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"$")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"4")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_4_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_5_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_5_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"%")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"5")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_5_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_6_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_6_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"^")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"6")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_6_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_7_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_7_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"&")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"7")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_7_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_8_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_8_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"*")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"8")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_8_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_9_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_9_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"(")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"9")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_9_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_0_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_0_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,")")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"0")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_0_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_dash_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_dash_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"_")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"-")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_dash_drag_drop  (GtkWidget       *widget,
                       GdkDragContext  *context,
                       gint             x,
                       gint             y,
                       guint            time,
                       gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_equal_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_equal_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"+")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"=")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_equal_drag_drop  (GtkWidget       *widget,
                        GdkDragContext  *context,
                        gint             x,
                        gint             y,
                        guint            time,
                        gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_q_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_q_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"Q")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"q")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_q_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_w_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_w_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"W")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"w")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_w_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_e_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_e_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"E")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"e")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_e_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_r_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_r_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"R")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"r")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_r_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_t_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_t_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"T")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"t")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_t_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_y_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_y_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"Y")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"y")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_y_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_u_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_u_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"U")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"u")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_u_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_i_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_i_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"I")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"i")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_i_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_o_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_o_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"O")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"o")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_o_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_p_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_p_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"P")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"p")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_p_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_sq_bra_left_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_sq_bra_left_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"{")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"[")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_sq_bra_left_drag_drop  (GtkWidget       *widget,
                              GdkDragContext  *context,
                              gint             x,
                              gint             y,
                              guint            time,
                              gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_sq_bra_right_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_sq_bra_right_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"}")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"]")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_sq_bra_right_drag_drop  (GtkWidget       *widget,
                               GdkDragContext  *context,
                               gint             x,
                               gint             y,
                               guint            time,
                               gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_win_slash_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_win_slash_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"|")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"\\")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_win_slash_drag_drop  (GtkWidget       *widget,
                            GdkDragContext  *context,
                            gint             x,
                            gint             y,
                            guint            time,
                            gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_a_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_a_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"A")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"a")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_a_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_s_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_s_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"S")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"s")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_s_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_d_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_d_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"D")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"d")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_d_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_f_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_f_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"F")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"f")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_f_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_g_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_g_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"G")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"g")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_g_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_h_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_h_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"H")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"h")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_h_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_j_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_j_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"J")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"j")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_j_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_k_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_k_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"K")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"k")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_k_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_l_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_l_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"L")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"l")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_l_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_semi_colon_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_semi_colon_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,":")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,";")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_semi_colon_drag_drop  (GtkWidget       *widget,
                             GdkDragContext  *context,
                             gint             x,
                             gint             y,
                             guint            time,
                             gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_quote_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_quote_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"\"")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"'")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_quote_drag_drop  (GtkWidget       *widget,
                        GdkDragContext  *context,
                        gint             x,
                        gint             y,
                        guint            time,
                        gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_z_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_z_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"Z")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"z")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_z_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_x_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_x_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"X")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"x")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_x_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_c_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_c_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"C")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"c")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_c_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_v_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_v_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"V")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"v")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_v_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_b_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_b_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"B")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"b")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_b_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_n_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_n_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"N")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"n")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_n_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_m_drag_data_get                  (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        GtkSelectionData *selection_data,
                                        guint target_type,
                                        guint time,
                                        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_m_drag_data_received             (GtkWidget       *widget,
                                        GdkDragContext  *context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *selection_data,
                                        guint            target_type,
                                        guint            time,
                                        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"M")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"m")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_m_drag_drop  (GtkWidget       *widget,
                    GdkDragContext  *context,
                    gint             x,
                    gint             y,
                    guint            time,
                    gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_coma_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_coma_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"<")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,",")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_coma_drag_drop  (GtkWidget       *widget,
                       GdkDragContext  *context,
                       gint             x,
                       gint             y,
                       guint            time,
                       gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_dot_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_dot_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,">")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,".")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_dot_drag_drop  (GtkWidget       *widget,
                      GdkDragContext  *context,
                      gint             x,
                      gint             y,
                      guint            time,
                      gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

void
on_bt_linux_slash_drag_data_get                  (GtkWidget       *widget,
        GdkDragContext  *context,
        GtkSelectionData *selection_data,
        guint target_type,
        guint time,
        gpointer         user_data)
{
    wchar_t ktxt[3];
    gchar *sbtn;

    g_assert (selection_data != NULL);

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);
    switch (target_type)
    {
    case TARGET_STRING:
        gtk_selection_data_set(selection_data,selection_data-> target,_DWORD,(guchar*) ktxt,sizeof (ktxt));
        break;

    case TARGET_ROOTWIN:
        g_print ("Dropped on the root window!\n");
        break;

    default:
        g_assert_not_reached ();
    }
}
void
on_bt_linux_slash_drag_data_received             (GtkWidget       *widget,
        GdkDragContext  *context,
        gint             x,
        gint             y,
        GtkSelectionData *selection_data,
        guint            target_type,
        guint            time,
        gpointer         user_data) {

    int ii=0;
    gchar   *_sdata;
    GtkButton *swidget;
    wchar_t ktxt[3];
    gchar *sbtn;
    char sbtname[2];
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));
    strcpy((char *)ktxt,sbtn);

    if ((selection_data != NULL) && (selection_data-> length >= 0))
    {
        if (context-> action == GDK_ACTION_ASK)
        {
            /* Ask the user to move or copy, then set the context action. */
        }

        if (context-> action == GDK_ACTION_MOVE)
            delete_selection_data = TRUE;

        switch (target_type)
        {
        case TARGET_STRING:
            memset(sbtname,'\0',2);
            _sdata = (gchar*)selection_data-> data;
            gtk_button_set_label((GtkButton *)widget,_sdata);
            swidget = (GtkButton *)gtk_drag_get_source_widget(context);
            gtk_button_set_label(swidget,(char *)ktxt);
            for (ii=0; ii<mapcnt; ii++)
            {
                if (strcmp(map[ii].keyvalue,_sdata)==0)
                {
                    strcpy(sbtname,map[ii].keyname);
                }
                if (gtk_toggle_button_get_active((GtkToggleButton *)caps_btn) || gtk_toggle_button_get_active((GtkToggleButton *)l_shift_btn) || gtk_toggle_button_get_active((GtkToggleButton *)r_shift_btn))
                {
                    if ( strcmp(map[ii].keyname,"?")==0)
                    {
                        strcpy(map[ii].keyvalue,_sdata);
                    }
                }
                else if (strcmp(map[ii].keyname,"/")==0)
                {
                    strcpy(map[ii].keyvalue,_sdata);
                }
            }
            for (ii=0; ii<mapcnt; ii++) {
                if (strcmp(map[ii].keyname,sbtname)==0) {
                    strcpy(map[ii].keyvalue,ktxt);
                }
            }
            dnd_success = TRUE;
            break;

        default:
            g_print ("nothing good");
        }
    }

    if (dnd_success == FALSE)
    {
        g_print ("DnD data transfer failed!\n");
    }
    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}
gboolean
on_bt_linux_slash_drag_drop  (GtkWidget       *widget,
                              GdkDragContext  *context,
                              gint             x,
                              gint             y,
                              guint            time,
                              gpointer         user_data) {

    gboolean        is_valid_drop_site;
    GdkAtom         target_type;

    is_valid_drop_site = TRUE;
    if (context-> targets)
    {
        gchar *sbtn;
        sbtn = (gchar *) gtk_button_get_label(GTK_BUTTON(widget));

        target_type = GDK_POINTER_TO_ATOM
                      (g_list_nth_data (context-> targets, TARGET_STRING));
        gtk_drag_get_data(widget,context,target_type,time);
    }
    else
    {
        is_valid_drop_site = FALSE;
    }

    return  is_valid_drop_site;
}

static void
_print_usage (FILE *fp, gchar *name)
{
    fprintf (fp,
             "Usage:\n"
             " %s --help            Show this message\n"
             "    --open -n          Set keymap file\n"
             "    --gen-xkb -g        Generate xkb keymap file\n"
             "    --advanced -a        Show advanced options\n",
             name);
}

GtkWidget*
create_IndicMapper (int argc, char **argv)
{
    gint       left, top;
    gchar  *ltoken;
    gint c;
    FILE *ofp;
    char *mname=NULL;
    IndicMapper = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (IndicMapper), "iok - Indic Onscreen Keyboard");
    gtk_window_set_position (GTK_WINDOW (IndicMapper), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (IndicMapper), 1000, -1);
    gtk_window_set_decorated(GTK_WINDOW(IndicMapper), TRUE);
    gtk_window_set_accept_focus(GTK_WINDOW(IndicMapper), FALSE);
    gtk_window_set_resizable (GTK_WINDOW (IndicMapper), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(IndicMapper), TRUE);

    g_signal_connect (G_OBJECT (IndicMapper), "destroy",
                      G_CALLBACK (gtk_main_quit),
                      NULL);
    localename = getenv("LANG");
    ltoken = strtok(localename, "_");

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (IndicMapper), vbox1);
    gtk_container_set_border_width (GTK_CONTAINER (vbox1), 3);

    hbox6 = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox6);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox6, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox6), 2);


    file_menu = gtk_menu_new ();    /* Don't need to show menus */

    /* Create the menu items */
    open_item = gtk_menu_item_new_with_mnemonic (_("Open"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, open_item, _("Open a Keymap that is not listed in a combobox"), NULL);

    save_item = gtk_menu_item_new_with_mnemonic (_("Save"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, save_item, _("Save current state of loaded keymap to a new keymap"), NULL);

    separator_item = gtk_separator_menu_item_new ();
    quit_item = gtk_menu_item_new_with_mnemonic (_("Quit"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, quit_item, _("Close iok application"), NULL);

    /* Add them to the menu */
    gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), open_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), save_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), separator_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), quit_item);

    /* Attach the callback functions to the activate signal */
    g_signal_connect_swapped (G_OBJECT (open_item), "activate",
                              G_CALLBACK (on_bt_opmap_clicked),
                              (gpointer) open_item);

    g_signal_connect_swapped (G_OBJECT (save_item), "activate",
                              G_CALLBACK (on_bt_wrmap_clicked),
                              (gpointer) save_item);

    /* We can attach the Quit menu item to our exit function */
    g_signal_connect_swapped (G_OBJECT (quit_item), "activate",
                              G_CALLBACK (gtk_main_quit),
                              (gpointer) quit_item);

    /* We do need to show menu items */
    gtk_widget_show (open_item);
    gtk_widget_show (save_item);
    gtk_widget_show (separator_item);
    gtk_widget_show (quit_item);

    keys_menu = gtk_menu_new ();    /* Don't need to show menus */

    /* Create the menu items */
    reassign_item = gtk_menu_item_new_with_mnemonic (_("Reassign"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, reassign_item, _("Reassign key value of some key to some other key"), NULL);

    swap_item = gtk_menu_item_new_with_mnemonic (_("Swap"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, swap_item, _("Swap key values between any 2 keys in loaded keymap"), NULL);

    /* Add them to the menu */
    gtk_menu_shell_append (GTK_MENU_SHELL (keys_menu), reassign_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (keys_menu), swap_item);

    /* Attach the callback functions to the activate signal */
    g_signal_connect_swapped (G_OBJECT (reassign_item), "activate",
                              G_CALLBACK (on_bt_reakey_clicked),
                              (gpointer) reassign_item);
    g_signal_connect_swapped (G_OBJECT (swap_item), "activate",
                              G_CALLBACK (on_bt_swapkey_clicked),
                              (gpointer) swap_item);


    gtk_widget_show (reassign_item);
    gtk_widget_show (swap_item);

    help_menu = gtk_menu_new ();    /* Don't need to show menus */

    /* Create the menu items */
    about_item = gtk_menu_item_new_with_mnemonic (_("About"));
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, about_item, _("About iok application"), NULL);


    /* Add them to the menu */
    gtk_menu_shell_append (GTK_MENU_SHELL (help_menu), about_item);

    /* Attach the callback functions to the activate signal */
    g_signal_connect_swapped (G_OBJECT (about_item), "activate",
                              G_CALLBACK (on_bt_about_clicked),
                              (gpointer) about_item);

    gtk_widget_show (about_item);

    menu_bar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (hbox6), menu_bar, TRUE, TRUE, 0);

    file_item = gtk_menu_item_new_with_mnemonic(_("Keymap"));
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), file_menu);
    gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), file_item);
    keys_item = gtk_menu_item_new_with_mnemonic(_("Keys"));
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (keys_item), keys_menu);
    gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), keys_item);
    help_item = gtk_menu_item_new_with_mnemonic(_("Help"));
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_item), help_menu);
    gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), help_item);

    gtk_widget_show (file_item);
    gtk_widget_show (keys_item);
    gtk_widget_show (help_item);

    /*Start row1 Esc,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PrtSc,SLock,Pause */
    hbox0 =gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox0);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox0, TRUE, TRUE, 0);
    table0 = gtk_table_new( 1, 30, TRUE );
    gtk_container_add( GTK_CONTAINER( hbox0 ), table0 );

    left = 4;
    top = 0;
    btn_esc = gtk_button_new_with_label( "Esc" );
    gtk_table_attach( GTK_TABLE( table0 ), btn_esc, 0, 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (btn_esc);

    bt_f1 = gtk_button_new_with_label( "F1" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f1, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f1);

    left += 2;
    bt_f2 = gtk_button_new_with_label( "F2" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f2, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f2);

    left += 2;
    bt_f3 = gtk_button_new_with_label( "F3" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f3, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f3);

    left += 2;
    bt_f4 = gtk_button_new_with_label( "F4" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f4, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f4);

    left += 3;
    bt_f5 = gtk_button_new_with_label( "F5" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f5, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f5);

    left += 2;
    bt_f6 = gtk_button_new_with_label( "F6" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f6, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f6);

    left += 2;
    bt_f7 = gtk_button_new_with_label( "F7" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f7, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f7);

    left += 2;
    bt_f8 = gtk_button_new_with_label( "F8" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f8, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f8);

    left += 3;
    bt_f9 = gtk_button_new_with_label( "F9" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f9, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f9);

    left += 2;
    bt_f10 = gtk_button_new_with_label( "F10" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f10, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f10);

    left += 2;
    bt_f11 = gtk_button_new_with_label( "F11" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f11, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f11);

    left += 2;
    bt_f12 = gtk_button_new_with_label( "F12" );
    gtk_table_attach( GTK_TABLE( table0 ), bt_f12, left, left+2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_f12);


    /*Start row2 ~,1,2,3,4,5,6,7,8,9,0,-,=,backspace*/
    left = 0;
    top = 0;

    hbox1 = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox1), 2);
    table1 = gtk_table_new( 1, 30, TRUE );
    gtk_container_add( GTK_CONTAINER( hbox1 ), table1 );

    bt_back_quote = gtk_button_new_with_label ("~\n`");
    gtk_table_attach( GTK_TABLE( table1 ), bt_back_quote, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_back_quote);

    bt_1 = gtk_button_new_with_label ("!\n1");
    gtk_table_attach( GTK_TABLE( table1 ), bt_1, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_1);

    bt_2 = gtk_button_new_with_label ("@\n2");
    gtk_table_attach( GTK_TABLE( table1 ), bt_2, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_2);

    bt_3 = gtk_button_new_with_label ("#\n3");
    gtk_table_attach( GTK_TABLE( table1 ), bt_3, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_3);

    bt_4 = gtk_button_new_with_label ("$\n4");
    gtk_table_attach( GTK_TABLE( table1 ), bt_4, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_4);

    bt_5 = gtk_button_new_with_label ("%\n5");
    gtk_table_attach( GTK_TABLE( table1 ), bt_5, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_5);

    bt_6 = gtk_button_new_with_label ("^\n6");
    gtk_table_attach( GTK_TABLE( table1 ), bt_6, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_6);

    bt_7 = gtk_button_new_with_label ("&\n7");
    gtk_table_attach( GTK_TABLE( table1 ), bt_7, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_7);

    bt_8 = gtk_button_new_with_label ("*\n8");
    gtk_table_attach( GTK_TABLE( table1 ), bt_8, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_8);

    bt_9 = gtk_button_new_with_label ("(\n9");
    gtk_table_attach( GTK_TABLE( table1 ), bt_9, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_9);

    bt_0 = gtk_button_new_with_label (")\n0");
    gtk_table_attach( GTK_TABLE( table1 ), bt_0, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_0);

    bt_dash = gtk_button_new_with_label ("_\n-");
    gtk_table_attach( GTK_TABLE( table1 ), bt_dash, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_dash);

    bt_equal = gtk_button_new_with_label ("+\n=");
    gtk_table_attach( GTK_TABLE( table1 ), bt_equal, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_equal);

    backspace_btn = gtk_button_new_with_mnemonic (_("Backspace"));
    gtk_widget_show (backspace_btn);
    gtk_table_attach( GTK_TABLE( table1 ), backspace_btn, 26, 30, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;


    /*  second row              */
    left = 3;
    hbox2 = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox2);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox2), 2);
    table2 = gtk_table_new( 1, 30, TRUE );
    gtk_container_add( GTK_CONTAINER( hbox2 ), table2 );

    tab_btn = gtk_button_new_with_mnemonic (_("Tab"));
    gtk_table_attach( GTK_TABLE( table2 ), tab_btn, 0, 3, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (tab_btn);

    bt_q = gtk_button_new_with_label ("Q\nq");
    gtk_table_attach( GTK_TABLE( table2 ), bt_q, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_q);

    bt_w = gtk_button_new_with_label ("W\nw");
    gtk_table_attach( GTK_TABLE( table2 ), bt_w, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_w);

    bt_e = gtk_button_new_with_label ("E\ne");
    gtk_table_attach( GTK_TABLE( table2 ), bt_e, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_e);

    bt_r = gtk_button_new_with_label ("R\nr");
    gtk_table_attach( GTK_TABLE( table2 ), bt_r, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_r);

    bt_t = gtk_button_new_with_label ("T\nt");
    gtk_table_attach( GTK_TABLE( table2 ), bt_t, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_t);

    bt_y = gtk_button_new_with_label ("Y\ny");
    gtk_table_attach( GTK_TABLE( table2 ), bt_y, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_y);

    bt_u = gtk_button_new_with_label ("U\nu");
    gtk_table_attach( GTK_TABLE( table2 ), bt_u, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_u);

    bt_i = gtk_button_new_with_label ("I\ni");
    gtk_table_attach( GTK_TABLE( table2 ), bt_i, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_i);

    bt_o = gtk_button_new_with_label ("O\no");
    gtk_table_attach( GTK_TABLE( table2 ), bt_o, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_o);

    bt_p = gtk_button_new_with_label ("P\np");
    gtk_table_attach( GTK_TABLE( table2 ), bt_p, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_p);

    bt_sq_bra_left = gtk_button_new_with_label ("{\n[");
    gtk_table_attach( GTK_TABLE( table2 ), bt_sq_bra_left, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_sq_bra_left);

    bt_sq_bra_right = gtk_button_new_with_label ("}\n]");
    gtk_table_attach( GTK_TABLE( table2 ), bt_sq_bra_right, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_sq_bra_right);

    bt_win_slash = gtk_button_new_with_label ("|\n\\");
    gtk_table_attach( GTK_TABLE( table2 ), bt_win_slash, 27, 30, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (bt_win_slash);

    /* Third row */
    left = 4;
    hbox3 = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox3);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox3), 2);
    table3 = gtk_table_new( 1, 30, TRUE );
    gtk_container_add( GTK_CONTAINER( hbox3 ), table3 );
    gtk_widget_show (table3);

    caps_btn = gtk_toggle_button_new_with_mnemonic (_("Caps"));
    gtk_table_attach( GTK_TABLE( table3 ), caps_btn, 0, 4, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (caps_btn);

    bt_a = gtk_button_new_with_label ("A\na");
    gtk_table_attach( GTK_TABLE( table3 ), bt_a, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_a);

    bt_s = gtk_button_new_with_label ("S\ns");
    gtk_table_attach( GTK_TABLE( table3 ), bt_s, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_s);

    bt_d = gtk_button_new_with_label ("D\nd");
    gtk_table_attach( GTK_TABLE( table3 ), bt_d, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_d);

    bt_f = gtk_button_new_with_label ("F\nf");
    gtk_table_attach( GTK_TABLE( table3 ), bt_f, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_f);

    bt_g = gtk_button_new_with_label ("G\ng");
    gtk_table_attach( GTK_TABLE( table3 ), bt_g, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_g);

    bt_h = gtk_button_new_with_label ("H\nh");
    gtk_table_attach( GTK_TABLE( table3 ), bt_h, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_h);

    bt_j = gtk_button_new_with_label ("J\nj");
    gtk_table_attach( GTK_TABLE( table3 ), bt_j, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_j);

    bt_k = gtk_button_new_with_label ("K\nk");
    gtk_table_attach( GTK_TABLE( table3 ), bt_k, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_k);

    bt_l = gtk_button_new_with_label ("L\nl");
    gtk_table_attach( GTK_TABLE( table3 ), bt_l, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_l);

    bt_semi_colon = gtk_button_new_with_label (":\n;");
    gtk_table_attach( GTK_TABLE( table3 ), bt_semi_colon, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_semi_colon);

    bt_quote = gtk_button_new_with_label ("\"\n'");
    gtk_table_attach( GTK_TABLE( table3 ), bt_quote, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_quote);

    enter_btn = gtk_button_new_with_mnemonic (_("Enter"));
    gtk_table_attach( GTK_TABLE( table3 ), enter_btn, 26, 30, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (enter_btn);

    /* Fourth Row */
    left = 5;
    hbox4 = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox4);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox4), 2);
    table4 = gtk_table_new( 1, 30, TRUE );
    gtk_container_add( GTK_CONTAINER( hbox4 ), table4 );

    l_shift_btn = gtk_toggle_button_new_with_mnemonic (_("Shift"));
    gtk_table_attach( GTK_TABLE( table4 ), l_shift_btn, 0, left  , top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (l_shift_btn);

    bt_z = gtk_button_new_with_label ("Z\nz");
    gtk_table_attach( GTK_TABLE( table4 ), bt_z, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_z);

    bt_x = gtk_button_new_with_label ("X\nx");
    gtk_table_attach( GTK_TABLE( table4 ), bt_x, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_x);

    bt_c = gtk_button_new_with_label ("C\nc");
    gtk_table_attach( GTK_TABLE( table4 ), bt_c, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_c);

    bt_v = gtk_button_new_with_label ("V\nv");
    gtk_table_attach( GTK_TABLE( table4 ), bt_v, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_v);

    bt_b = gtk_button_new_with_label ("B\nb");
    gtk_table_attach( GTK_TABLE( table4 ), bt_b, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_b);

    bt_n = gtk_button_new_with_label ("N\nn");
    gtk_table_attach( GTK_TABLE( table4 ), bt_n, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_n);

    bt_m = gtk_button_new_with_label ("M\nm");
    gtk_table_attach( GTK_TABLE( table4 ), bt_m, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_m);

    bt_coma = gtk_button_new_with_label ("<\n,");
    gtk_table_attach( GTK_TABLE( table4 ), bt_coma, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_coma);

    bt_dot = gtk_button_new_with_label (">\n.");
    gtk_table_attach( GTK_TABLE( table4 ), bt_dot, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_dot);

    bt_linux_slash = gtk_button_new_with_label ("?\n/");
    gtk_table_attach( GTK_TABLE( table4 ), bt_linux_slash, left, left + 2, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    left += 2;
    gtk_widget_show (bt_linux_slash);

    r_shift_btn = gtk_toggle_button_new_with_mnemonic (_("Shift"));
    gtk_table_attach( GTK_TABLE( table4 ), r_shift_btn, 25, 30, top, top + 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0 );
    gtk_widget_show (r_shift_btn);

    hbox9 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox9);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

    keyswitch_btn = gtk_button_new_with_label ("to English");
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, keyswitch_btn, _("Change to English Keyboard"), NULL);
    gtk_box_pack_start (GTK_BOX (hbox9), keyswitch_btn, TRUE, TRUE, 0);

    space_btn = gtk_button_new_with_mnemonic (_("Space"));
    gtk_widget_show (space_btn);
    gtk_box_pack_start (GTK_BOX (hbox9), space_btn, TRUE, TRUE, 0);
    gtk_widget_set_size_request (space_btn, 325, -1);

    combo_mim = gtk_combo_box_new_text ();
    tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, combo_mim, _("Select the language in which you want to write. This list contains keymaps in order as inscript then user defined then xkb and then any keymaps opened that are not default listed"), NULL);
    gtk_box_pack_start (GTK_BOX (hbox9), combo_mim, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (combo_mim), "changed",
                      G_CALLBACK (on_combo_mim_changed),
                      (gpointer) combo_mim);
    add_mimfilelist();

    ofp=fopen("/usr/share/m17n/ne-trad.mim","r");
    if (ofp!= NULL)
    {
        mname=(char *) malloc(strlen("/usr/share/m17n/ne-trad.mim")+1 * sizeof(char));
        strcpy(mname,"/usr/share/m17n/ne-trad.mim");
        open_file(mname);
    }
    ofp=fopen("/usr/share/m17n/ne-rom.mim","r");
    if (ofp!= NULL)
    {
        mname=(char *) malloc(strlen("/usr/share/m17n/ne-rom.mim")+1 * sizeof(char));
        strcpy(mname,"/usr/share/m17n/ne-rom.mim");
        open_file(mname);
    }
    add_xkbmaplist();
    gtk_combo_box_set_active  ((GtkComboBox *)combo_mim,0);


    while ((c = getopt (argc, argv, "ag:n:?")) != -1) {
        switch (c) {
        case 'a':
            gtk_widget_show (menu_bar);
            gtk_widget_show (combo_mim);
            gtk_widget_show (keyswitch_btn);
            enable_dnd=1;
            break;
        case 'g':
            ltoken=optarg;
            set_default_map_name(ltoken);
            print_xkb_map();
            exit(0);
            break;
        case 'n':
            ltoken=optarg;
            break;
        case '?':
        default:
            _print_usage (stderr, argv[0]);
            exit (EXIT_FAILURE);
        }
    }

    gtk_widget_show (table0);
    gtk_widget_show (table1);
    gtk_widget_show (table2);
    gtk_widget_show (table3);
    gtk_widget_show (table4);

    if (strcmp(ltoken,"en")!= 0)
    {
        set_default_map_name(ltoken);
        gtk_widget_show (keyswitch_btn);
    }
    else
    {
        english_mappings();
        gtk_widget_show (combo_mim);
        gtk_widget_show (keyswitch_btn);
    }

    g_signal_connect (G_OBJECT (bt_back_quote), "clicked",
                      G_CALLBACK (on_bt_back_quote_clicked), (gpointer) bt_back_quote);
    g_signal_connect (G_OBJECT (bt_1), "clicked",
                      G_CALLBACK (on_bt_1_clicked), (gpointer) bt_1);
    g_signal_connect (G_OBJECT (bt_2), "clicked",
                      G_CALLBACK (on_bt_2_clicked), (gpointer) bt_2);
    g_signal_connect (G_OBJECT (bt_3), "clicked",
                      G_CALLBACK (on_bt_3_clicked), (gpointer) bt_3);
    g_signal_connect (G_OBJECT (bt_4), "clicked",
                      G_CALLBACK (on_bt_4_clicked), (gpointer) bt_4);
    g_signal_connect (G_OBJECT (bt_5), "clicked",
                      G_CALLBACK (on_bt_5_clicked), (gpointer) bt_5);
    g_signal_connect (G_OBJECT (bt_6), "clicked",
                      G_CALLBACK (on_bt_6_clicked), (gpointer) bt_6);
    g_signal_connect (G_OBJECT (bt_7), "clicked",
                      G_CALLBACK (on_bt_7_clicked), (gpointer) bt_7);
    g_signal_connect (G_OBJECT (bt_8), "clicked",
                      G_CALLBACK (on_bt_8_clicked), (gpointer) bt_8);
    g_signal_connect (G_OBJECT (bt_9), "clicked",
                      G_CALLBACK (on_bt_9_clicked), (gpointer) bt_9);
    g_signal_connect (G_OBJECT (bt_0), "clicked",
                      G_CALLBACK (on_bt_0_clicked), (gpointer) bt_0);
    g_signal_connect (G_OBJECT (bt_dash), "clicked",
                      G_CALLBACK (on_bt_dash_clicked), (gpointer) bt_dash);
    g_signal_connect (G_OBJECT (bt_equal), "clicked",
                      G_CALLBACK (on_bt_equal_clicked), (gpointer) bt_equal);

    g_signal_connect (G_OBJECT (bt_q), "clicked",
                      G_CALLBACK (on_bt_q_clicked), (gpointer) bt_q);
    g_signal_connect (G_OBJECT (bt_w), "clicked",
                      G_CALLBACK (on_bt_w_clicked), (gpointer) bt_w);
    g_signal_connect (G_OBJECT (bt_e), "clicked",
                      G_CALLBACK (on_bt_e_clicked), (gpointer) bt_e);
    g_signal_connect (G_OBJECT (bt_r), "clicked",
                      G_CALLBACK (on_bt_r_clicked), (gpointer) bt_r);
    g_signal_connect (G_OBJECT (bt_t), "clicked",
                      G_CALLBACK (on_bt_t_clicked), (gpointer) bt_t);
    g_signal_connect (G_OBJECT (bt_y), "clicked",
                      G_CALLBACK (on_bt_y_clicked), (gpointer) bt_y);
    g_signal_connect (G_OBJECT (bt_u), "clicked",
                      G_CALLBACK (on_bt_u_clicked), (gpointer) bt_u);
    g_signal_connect (G_OBJECT (bt_i), "clicked",
                      G_CALLBACK (on_bt_i_clicked), (gpointer) bt_i);
    g_signal_connect (G_OBJECT (bt_o), "clicked",
                      G_CALLBACK (on_bt_o_clicked), (gpointer) bt_o);
    g_signal_connect (G_OBJECT (bt_p), "clicked",
                      G_CALLBACK (on_bt_p_clicked), (gpointer) bt_p);
    g_signal_connect (G_OBJECT (bt_sq_bra_left), "clicked",
                      G_CALLBACK (on_bt_sq_bra_left_clicked), (gpointer) bt_sq_bra_left);
    g_signal_connect (G_OBJECT (bt_sq_bra_right), "clicked",
                      G_CALLBACK (on_bt_sq_bra_right_clicked), (gpointer) bt_sq_bra_right);
    g_signal_connect (G_OBJECT (bt_win_slash), "clicked",
                      G_CALLBACK (on_bt_win_slash_clicked), (gpointer) bt_win_slash);

    g_signal_connect (G_OBJECT (bt_a), "clicked",
                      G_CALLBACK (on_bt_a_clicked), (gpointer) bt_a);
    g_signal_connect (G_OBJECT (bt_s), "clicked",
                      G_CALLBACK (on_bt_s_clicked), (gpointer) bt_s);
    g_signal_connect (G_OBJECT (bt_d), "clicked",
                      G_CALLBACK (on_bt_d_clicked), (gpointer) bt_d);
    g_signal_connect (G_OBJECT (bt_f), "clicked",
                      G_CALLBACK (on_bt_f_clicked), (gpointer) bt_f);
    g_signal_connect (G_OBJECT (bt_g), "clicked",
                      G_CALLBACK (on_bt_g_clicked), (gpointer) bt_g);
    g_signal_connect (G_OBJECT (bt_h), "clicked",
                      G_CALLBACK (on_bt_h_clicked), (gpointer) bt_h);
    g_signal_connect (G_OBJECT (bt_j), "clicked",
                      G_CALLBACK (on_bt_j_clicked), (gpointer) bt_j);
    g_signal_connect (G_OBJECT (bt_k), "clicked",
                      G_CALLBACK (on_bt_k_clicked), (gpointer) bt_k);
    g_signal_connect (G_OBJECT (bt_l), "clicked",
                      G_CALLBACK (on_bt_l_clicked), (gpointer) bt_l);
    g_signal_connect (G_OBJECT (bt_semi_colon), "clicked",
                      G_CALLBACK (on_bt_semi_colon_clicked), (gpointer) bt_semi_colon);
    g_signal_connect (G_OBJECT (bt_quote), "clicked",
                      G_CALLBACK (on_bt_quote_clicked), (gpointer) bt_quote);


    g_signal_connect (G_OBJECT (bt_z), "clicked",
                      G_CALLBACK (on_bt_z_clicked), (gpointer) bt_z);
    g_signal_connect (G_OBJECT (bt_x), "clicked",
                      G_CALLBACK (on_bt_x_clicked), (gpointer) bt_x);
    g_signal_connect (G_OBJECT (bt_c), "clicked",
                      G_CALLBACK (on_bt_c_clicked), (gpointer) bt_c);
    g_signal_connect (G_OBJECT (bt_v), "clicked",
                      G_CALLBACK (on_bt_v_clicked), (gpointer) bt_v);
    g_signal_connect (G_OBJECT (bt_b), "clicked",
                      G_CALLBACK (on_bt_b_clicked), (gpointer) bt_b);
    g_signal_connect (G_OBJECT (bt_n), "clicked",
                      G_CALLBACK (on_bt_n_clicked), (gpointer) bt_n);
    g_signal_connect (G_OBJECT (bt_m), "clicked",
                      G_CALLBACK (on_bt_m_clicked), (gpointer) bt_m);
    g_signal_connect (G_OBJECT (bt_coma), "clicked",
                      G_CALLBACK (on_bt_coma_clicked), (gpointer) bt_coma);
    g_signal_connect (G_OBJECT (bt_dot), "clicked",
                      G_CALLBACK (on_bt_dot_clicked), (gpointer) bt_dot);
    g_signal_connect (G_OBJECT (bt_linux_slash), "clicked",
                      G_CALLBACK (on_bt_linux_slash_clicked), (gpointer) bt_linux_slash);
    g_signal_connect (G_OBJECT (caps_btn), "toggled",
                      G_CALLBACK (on_caps_btn_clicked), (gpointer) caps_btn);
    g_signal_connect (G_OBJECT (l_shift_btn), "toggled",
                      G_CALLBACK (on_l_shift_btn_clicked), (gpointer) l_shift_btn);
    g_signal_connect (G_OBJECT (r_shift_btn), "toggled",
                      G_CALLBACK (on_r_shift_btn_clicked), (gpointer) r_shift_btn);
    g_signal_connect (G_OBJECT (enter_btn), "clicked",
                      G_CALLBACK (on_enter_btn_clicked), (gpointer) enter_btn);
    g_signal_connect (G_OBJECT (backspace_btn), "clicked",
                      G_CALLBACK (on_backspace_btn_clicked), (gpointer) backspace_btn);
    g_signal_connect (G_OBJECT (tab_btn), "clicked",
                      G_CALLBACK (on_tab_btn_clicked), (gpointer) tab_btn);
    g_signal_connect (G_OBJECT (keyswitch_btn), "clicked",
                      G_CALLBACK (on_keyswitch_btn_clicked), (gpointer) keyswitch_btn);
    g_signal_connect (G_OBJECT (space_btn), "clicked",
                      G_CALLBACK (on_space_btn_clicked), (gpointer) space_btn);

    g_signal_connect (G_OBJECT (btn_esc), "clicked",
                      G_CALLBACK (on_esc_btn_clicked), (gpointer) btn_esc);
    g_signal_connect (G_OBJECT (bt_f1), "clicked",
                      G_CALLBACK (on_bt_f1_clicked), (gpointer) bt_f1);
    g_signal_connect (G_OBJECT (bt_f2), "clicked",
                      G_CALLBACK (on_bt_f2_clicked), (gpointer) bt_f2);
    g_signal_connect (G_OBJECT (bt_f3), "clicked",
                      G_CALLBACK (on_bt_f3_clicked), (gpointer) bt_f3);
    g_signal_connect (G_OBJECT (bt_f4), "clicked",
                      G_CALLBACK (on_bt_f4_clicked), (gpointer) bt_f4);
    g_signal_connect (G_OBJECT (bt_f5), "clicked",
                      G_CALLBACK (on_bt_f5_clicked), (gpointer) bt_f5);
    g_signal_connect (G_OBJECT (bt_f6), "clicked",
                      G_CALLBACK (on_bt_f6_clicked), (gpointer) bt_f6);
    g_signal_connect (G_OBJECT (bt_f7), "clicked",
                      G_CALLBACK (on_bt_f7_clicked), (gpointer) bt_f7);
    g_signal_connect (G_OBJECT (bt_f8), "clicked",
                      G_CALLBACK (on_bt_f8_clicked), (gpointer) bt_f8);
    g_signal_connect (G_OBJECT (bt_f9), "clicked",
                      G_CALLBACK (on_bt_f9_clicked), (gpointer) bt_f9);
    g_signal_connect (G_OBJECT (bt_f10), "clicked",
                      G_CALLBACK (on_bt_f10_clicked), (gpointer) bt_f10);
    g_signal_connect (G_OBJECT (bt_f11), "clicked",
                      G_CALLBACK (on_bt_f11_clicked), (gpointer) bt_f11);
    g_signal_connect (G_OBJECT (bt_f12), "clicked",
                      G_CALLBACK (on_bt_f12_clicked), (gpointer) bt_f12);

    if (enable_dnd)
    {
        gtk_drag_dest_set
        (
            bt_back_quote,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_back_quote,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_back_quote, "drag_data_get",
                          G_CALLBACK (on_bt_back_quote_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_back_quote, "drag_data_received",
                          G_CALLBACK (on_bt_back_quote_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_back_quote, "drag_drop",
                          G_CALLBACK (on_bt_back_quote_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_1,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_1,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_1, "drag_data_get",
                          G_CALLBACK (on_bt_1_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_1, "drag_data_received",
                          G_CALLBACK (on_bt_1_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_1, "drag_drop",
                          G_CALLBACK (on_bt_1_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_2,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_2,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_2, "drag_data_get",
                          G_CALLBACK (on_bt_2_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_2, "drag_data_received",
                          G_CALLBACK (on_bt_2_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_2, "drag_drop",
                          G_CALLBACK (on_bt_2_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_3,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_3,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_3, "drag_data_get",
                          G_CALLBACK (on_bt_3_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_3, "drag_data_received",
                          G_CALLBACK (on_bt_3_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_3, "drag_drop",
                          G_CALLBACK (on_bt_3_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_4,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_4,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_4, "drag_data_get",
                          G_CALLBACK (on_bt_4_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_4, "drag_data_received",
                          G_CALLBACK (on_bt_4_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_4, "drag_drop",
                          G_CALLBACK (on_bt_4_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_5,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_5,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_5, "drag_data_get",
                          G_CALLBACK (on_bt_5_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_5, "drag_data_received",
                          G_CALLBACK (on_bt_5_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_5, "drag_drop",
                          G_CALLBACK (on_bt_5_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_6,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_6,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_6, "drag_data_get",
                          G_CALLBACK (on_bt_6_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_6, "drag_data_received",
                          G_CALLBACK (on_bt_6_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_6, "drag_drop",
                          G_CALLBACK (on_bt_6_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_7,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_7,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_7, "drag_data_get",
                          G_CALLBACK (on_bt_7_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_7, "drag_data_received",
                          G_CALLBACK (on_bt_7_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_7, "drag_drop",
                          G_CALLBACK (on_bt_7_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_8,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_8,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_8, "drag_data_get",
                          G_CALLBACK (on_bt_8_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_8, "drag_data_received",
                          G_CALLBACK (on_bt_8_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_8, "drag_drop",
                          G_CALLBACK (on_bt_8_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_9,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_9,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_9, "drag_data_get",
                          G_CALLBACK (on_bt_9_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_9, "drag_data_received",
                          G_CALLBACK (on_bt_9_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_9, "drag_drop",
                          G_CALLBACK (on_bt_9_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_0,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_0,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_0, "drag_data_get",
                          G_CALLBACK (on_bt_0_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_0, "drag_data_received",
                          G_CALLBACK (on_bt_0_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_0, "drag_drop",
                          G_CALLBACK (on_bt_0_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_dash,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_dash,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_dash, "drag_data_get",
                          G_CALLBACK (on_bt_dash_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_dash, "drag_data_received",
                          G_CALLBACK (on_bt_dash_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_dash, "drag_drop",
                          G_CALLBACK (on_bt_dash_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_equal,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_equal,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_equal, "drag_data_get",
                          G_CALLBACK (on_bt_equal_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_equal, "drag_data_received",
                          G_CALLBACK (on_bt_equal_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_equal, "drag_drop",
                          G_CALLBACK (on_bt_equal_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_q,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_q,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_q, "drag_data_get",
                          G_CALLBACK (on_bt_q_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_q, "drag_data_received",
                          G_CALLBACK (on_bt_q_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_q, "drag_drop",
                          G_CALLBACK (on_bt_q_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_w,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_w,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_w, "drag_data_get",
                          G_CALLBACK (on_bt_w_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_w, "drag_data_received",
                          G_CALLBACK (on_bt_w_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_w, "drag_drop",
                          G_CALLBACK (on_bt_w_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_e,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_e,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_e, "drag_data_get",
                          G_CALLBACK (on_bt_e_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_e, "drag_data_received",
                          G_CALLBACK (on_bt_e_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_e, "drag_drop",
                          G_CALLBACK (on_bt_e_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_r,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_r,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_r, "drag_data_get",
                          G_CALLBACK (on_bt_r_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_r, "drag_data_received",
                          G_CALLBACK (on_bt_r_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_r, "drag_drop",
                          G_CALLBACK (on_bt_r_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_t,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_t,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_t, "drag_data_get",
                          G_CALLBACK (on_bt_t_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_t, "drag_data_received",
                          G_CALLBACK (on_bt_t_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_t, "drag_drop",
                          G_CALLBACK (on_bt_t_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_y,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_y,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_y, "drag_data_get",
                          G_CALLBACK (on_bt_y_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_y, "drag_data_received",
                          G_CALLBACK (on_bt_y_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_y, "drag_drop",
                          G_CALLBACK (on_bt_y_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_u,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_u,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_u, "drag_data_get",
                          G_CALLBACK (on_bt_u_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_u, "drag_data_received",
                          G_CALLBACK (on_bt_u_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_u, "drag_drop",
                          G_CALLBACK (on_bt_u_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_i,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_i,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_i, "drag_data_get",
                          G_CALLBACK (on_bt_i_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_i, "drag_data_received",
                          G_CALLBACK (on_bt_i_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_i, "drag_drop",
                          G_CALLBACK (on_bt_i_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_o,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_o,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_o, "drag_data_get",
                          G_CALLBACK (on_bt_o_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_o, "drag_data_received",
                          G_CALLBACK (on_bt_o_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_o, "drag_drop",
                          G_CALLBACK (on_bt_o_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_p,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_p,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_p, "drag_data_get",
                          G_CALLBACK (on_bt_p_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_p, "drag_data_received",
                          G_CALLBACK (on_bt_p_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_p, "drag_drop",
                          G_CALLBACK (on_bt_p_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_sq_bra_left,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_sq_bra_left,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_sq_bra_left, "drag_data_get",
                          G_CALLBACK (on_bt_sq_bra_left_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_sq_bra_left, "drag_data_received",
                          G_CALLBACK (on_bt_sq_bra_left_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_sq_bra_left, "drag_drop",
                          G_CALLBACK (on_bt_sq_bra_left_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_sq_bra_right,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_sq_bra_right,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_sq_bra_right, "drag_data_get",
                          G_CALLBACK (on_bt_sq_bra_right_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_sq_bra_right, "drag_data_received",
                          G_CALLBACK (on_bt_sq_bra_right_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_sq_bra_right, "drag_drop",
                          G_CALLBACK (on_bt_sq_bra_right_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_win_slash,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_win_slash,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_win_slash, "drag_data_get",
                          G_CALLBACK (on_bt_win_slash_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_win_slash, "drag_data_received",
                          G_CALLBACK (on_bt_win_slash_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_win_slash, "drag_drop",
                          G_CALLBACK (on_bt_win_slash_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_a,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_a,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_a, "drag_data_get",
                          G_CALLBACK (on_bt_a_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_a, "drag_data_received",
                          G_CALLBACK (on_bt_a_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_a, "drag_drop",
                          G_CALLBACK (on_bt_a_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_s,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_s,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_s, "drag_data_get",
                          G_CALLBACK (on_bt_s_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_s, "drag_data_received",
                          G_CALLBACK (on_bt_s_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_s, "drag_drop",
                          G_CALLBACK (on_bt_s_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_d,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_d,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_d, "drag_data_get",
                          G_CALLBACK (on_bt_d_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_d, "drag_data_received",
                          G_CALLBACK (on_bt_d_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_d, "drag_drop",
                          G_CALLBACK (on_bt_d_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_f,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_f,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_f, "drag_data_get",
                          G_CALLBACK (on_bt_f_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_f, "drag_data_received",
                          G_CALLBACK (on_bt_f_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_f, "drag_drop",
                          G_CALLBACK (on_bt_f_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_g,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_g,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_g, "drag_data_get",
                          G_CALLBACK (on_bt_g_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_g, "drag_data_received",
                          G_CALLBACK (on_bt_g_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_g, "drag_drop",
                          G_CALLBACK (on_bt_g_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_h,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_h,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_h, "drag_data_get",
                          G_CALLBACK (on_bt_h_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_h, "drag_data_received",
                          G_CALLBACK (on_bt_h_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_h, "drag_drop",
                          G_CALLBACK (on_bt_h_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_j,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_j,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_j, "drag_data_get",
                          G_CALLBACK (on_bt_j_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_j, "drag_data_received",
                          G_CALLBACK (on_bt_j_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_j, "drag_drop",
                          G_CALLBACK (on_bt_j_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_k,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_k,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_k, "drag_data_get",
                          G_CALLBACK (on_bt_k_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_k, "drag_data_received",
                          G_CALLBACK (on_bt_k_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_k, "drag_drop",
                          G_CALLBACK (on_bt_k_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_l,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_l,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_l, "drag_data_get",
                          G_CALLBACK (on_bt_l_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_l, "drag_data_received",
                          G_CALLBACK (on_bt_l_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_l, "drag_drop",
                          G_CALLBACK (on_bt_l_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_semi_colon,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_semi_colon,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_semi_colon, "drag_data_get",
                          G_CALLBACK (on_bt_semi_colon_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_semi_colon, "drag_data_received",
                          G_CALLBACK (on_bt_semi_colon_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_semi_colon, "drag_drop",
                          G_CALLBACK (on_bt_semi_colon_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_quote,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_quote,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_quote, "drag_data_get",
                          G_CALLBACK (on_bt_quote_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_quote, "drag_data_received",
                          G_CALLBACK (on_bt_quote_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_quote, "drag_drop",
                          G_CALLBACK (on_bt_quote_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_z,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_z,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_z, "drag_data_get",
                          G_CALLBACK (on_bt_z_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_z, "drag_data_received",
                          G_CALLBACK (on_bt_z_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_z, "drag_drop",
                          G_CALLBACK (on_bt_z_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_x,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_x,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_x, "drag_data_get",
                          G_CALLBACK (on_bt_x_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_x, "drag_data_received",
                          G_CALLBACK (on_bt_x_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_x, "drag_drop",
                          G_CALLBACK (on_bt_x_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_c,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_c,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_c, "drag_data_get",
                          G_CALLBACK (on_bt_c_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_c, "drag_data_received",
                          G_CALLBACK (on_bt_c_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_c, "drag_drop",
                          G_CALLBACK (on_bt_c_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_v,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_v,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_v, "drag_data_get",
                          G_CALLBACK (on_bt_v_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_v, "drag_data_received",
                          G_CALLBACK (on_bt_v_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_v, "drag_drop",
                          G_CALLBACK (on_bt_v_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_b,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_b,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_b, "drag_data_get",
                          G_CALLBACK (on_bt_b_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_b, "drag_data_received",
                          G_CALLBACK (on_bt_b_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_b, "drag_drop",
                          G_CALLBACK (on_bt_b_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_n,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_n,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_n, "drag_data_get",
                          G_CALLBACK (on_bt_n_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_n, "drag_data_received",
                          G_CALLBACK (on_bt_n_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_n, "drag_drop",
                          G_CALLBACK (on_bt_n_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_m,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_m,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_m, "drag_data_get",
                          G_CALLBACK (on_bt_m_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_m, "drag_data_received",
                          G_CALLBACK (on_bt_m_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_m, "drag_drop",
                          G_CALLBACK (on_bt_m_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_coma,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_coma,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_coma, "drag_data_get",
                          G_CALLBACK (on_bt_coma_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_coma, "drag_data_received",
                          G_CALLBACK (on_bt_coma_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_coma, "drag_drop",
                          G_CALLBACK (on_bt_coma_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_dot,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_dot,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_dot, "drag_data_get",
                          G_CALLBACK (on_bt_dot_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_dot, "drag_data_received",
                          G_CALLBACK (on_bt_dot_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_dot, "drag_drop",
                          G_CALLBACK (on_bt_dot_drag_drop),
                          NULL);

        gtk_drag_dest_set
        (
            bt_linux_slash,
            GTK_DEST_DEFAULT_MOTION
            | GTK_DEST_DEFAULT_HIGHLIGHT,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );

        gtk_drag_source_set
        (
            bt_linux_slash,
            GDK_BUTTON1_MASK,
            target_list,
            n_targets,
            GDK_ACTION_COPY
        );
        g_signal_connect ((gpointer) bt_linux_slash, "drag_data_get",
                          G_CALLBACK (on_bt_linux_slash_drag_data_get),
                          NULL);
        g_signal_connect ((gpointer) bt_linux_slash, "drag_data_received",
                          G_CALLBACK (on_bt_linux_slash_drag_data_received),
                          NULL);
        g_signal_connect ((gpointer) bt_linux_slash, "drag_drop",
                          G_CALLBACK (on_bt_linux_slash_drag_drop),
                          NULL);
    }

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (IndicMapper, IndicMapper, "IndicMapper");
    GLADE_HOOKUP_OBJECT (IndicMapper, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT (IndicMapper, hbox1, "hbox1");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_back_quote, "bt_back_quote");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_1, "bt_1");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_2, "bt_2");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_3, "bt_3");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_4, "bt_4");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_5, "bt_5");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_6, "bt_6");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_7, "bt_7");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_8, "bt_8");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_9, "bt_9");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_0, "bt_0");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_dash, "bt_dash");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_equal, "bt_equal");
    GLADE_HOOKUP_OBJECT (IndicMapper, backspace_btn, "backspace_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, hbox2, "hbox2");
    GLADE_HOOKUP_OBJECT (IndicMapper, tab_btn, "tab_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_q, "bt_q");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_w, "bt_w");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_e, "bt_e");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_r, "bt_r");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_t, "bt_t");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_y, "bt_y");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_u, "bt_u");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_i, "bt_i");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_o, "bt_o");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_p, "bt_p");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_sq_bra_left, "bt_sq_bra_left");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_sq_bra_right, "bt_sq_bra_right");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_win_slash, "bt_win_slash");
    GLADE_HOOKUP_OBJECT (IndicMapper, hbox3, "hbox3");
    GLADE_HOOKUP_OBJECT (IndicMapper, caps_btn, "caps_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_a, "bt_a");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_s, "bt_s");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_d, "bt_d");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_f, "bt_f");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_g, "bt_g");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_h, "bt_h");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_j, "bt_j");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_k, "bt_k");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_l, "bt_l");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_semi_colon, "bt_semi_colon");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_quote, "bt_quote");
    GLADE_HOOKUP_OBJECT (IndicMapper, enter_btn, "enter_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT (IndicMapper, l_shift_btn, "l_shift_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_z, "bt_z");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_x, "bt_x");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_c, "bt_c");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_v, "bt_v");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_b, "bt_b");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_n, "bt_n");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_m, "bt_m");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_coma, "bt_coma");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_dot, "bt_dot");
    GLADE_HOOKUP_OBJECT (IndicMapper, bt_linux_slash, "bt_linux_slash");
    GLADE_HOOKUP_OBJECT (IndicMapper, r_shift_btn, "r_shift_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, hbox9, "hbox9");
    GLADE_HOOKUP_OBJECT (IndicMapper, space_btn, "space_btn");
    GLADE_HOOKUP_OBJECT (IndicMapper, combo_mim, "combo_mim");

    return IndicMapper;
}


GtkWidget*
create_IndicMapper_aboutdialog (void)
{
    GtkWidget *IndicMapper_aboutdialog;
    const gchar *authors[] = {
        "Parag Nemade <panemade@gmail.com>",
        NULL
    };
    /* TRANSLATORS: Replace this string with your names, one name per line. */
    gchar *translators = _("translator-credits");

    IndicMapper_aboutdialog = gtk_about_dialog_new ();
    gtk_container_set_border_width (GTK_CONTAINER (IndicMapper_aboutdialog), 5);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (IndicMapper_aboutdialog), TRUE);
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (IndicMapper_aboutdialog), VERSION);
    gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (IndicMapper_aboutdialog), _("iok"));
    gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (IndicMapper_aboutdialog), _("GPLv2 or later\n"));
    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (IndicMapper_aboutdialog), authors);
    gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (IndicMapper_aboutdialog), translators);

    g_signal_connect (IndicMapper_aboutdialog, "response", G_CALLBACK (gtk_widget_destroy), IndicMapper_aboutdialog);


    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (IndicMapper_aboutdialog, IndicMapper_aboutdialog, "IndicMapper_aboutdialog");

    return IndicMapper_aboutdialog;
}


