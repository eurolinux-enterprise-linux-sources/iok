
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

#include <gtk/gtk.h>
#include <unique/unique.h>
#include <getopt.h>
#include <stdlib.h>

#include "interface.h"
#include "keys.h"
#include "support.h"
#include <string.h>

enum {
    COMMAND_0,
    COMMAND_OPT,
};

GtkWidget *IndicMapper = NULL;

static UniqueResponse
unique_app_message_cb (UniqueApp *app, gint command, UniqueMessageData *message_data,
                       guint time, gpointer user_data)
{
    gchar *message = NULL;

    /* raise the window */
    gtk_window_present_with_time (GTK_WINDOW (IndicMapper), time);

    switch (command)
    {
    case COMMAND_OPT:
        message = unique_message_data_get_text (message_data);
        set_default_map_name(message);
        break;
    default:
        break;
    }

    g_free (message);

    return UNIQUE_RESPONSE_OK;
}


int
main (int argc, char *argv[])
{
    UniqueApp *unique_app = NULL;

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

    gtk_set_locale ();
    gtk_init (&argc, &argv);

    add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

    unique_app = unique_app_new_with_commands ("org.gnome.MainMenu", NULL,
                 "opt", COMMAND_OPT, NULL);

    if (unique_app_is_running (unique_app))
    {
        UniqueMessageData *message = NULL;
        UniqueResponse response;
        gint command;

        char c1;
        while ((c1 = getopt (argc, argv, "n:")) != -1) {
            switch (c1) {
            case 'n':
                command = COMMAND_OPT;
                message = unique_message_data_new ();
                unique_message_data_set (message, (const guchar *) optarg, strlen(optarg));
                break;
            default:
                exit (0);
            }
        }
        if (message)
        {
            response = unique_app_send_message (unique_app, command, message);
            unique_message_data_free (message);
        }
        else
            response = unique_app_send_message (unique_app, command, NULL);

        g_object_unref (unique_app);

        return 0;
    }
    IndicMapper = create_IndicMapper (argc,argv);
    gtk_widget_show (IndicMapper);
    g_signal_connect (unique_app, "message-received", G_CALLBACK (unique_app_message_cb), IndicMapper);

    key_event_init();
    gtk_main ();
    g_object_unref (unique_app);

    return 0;
}

