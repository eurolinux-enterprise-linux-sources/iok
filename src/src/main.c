
/*
 *
 * iok - Virtual onscreen keyboard for Indic Inscript keymaps.
 * Copyright (C) 2008-2012 Parag Nemade <pnemade AT redhat DOT com>
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

#include "interface.h"
#include "keys.h"
#include "support.h"

#define COMMAND_NEW_INSTANCE 1

static UniqueResponse
unique_app_message_cb (UniqueApp *app, gint command, UniqueMessageData *data,
                      guint time, gpointer user_data)
{
       GtkWidget *window = user_data;
       gboolean  visible;

       if (command != COMMAND_NEW_INSTANCE)
               return UNIQUE_RESPONSE_PASSTHROUGH;

               gtk_window_set_screen (GTK_WINDOW (window),
                                      unique_message_data_get_screen (data));
               gtk_window_set_startup_id (GTK_WINDOW (window),
                                          unique_message_data_get_startup_id (data));
               gtk_window_present_with_time (GTK_WINDOW (window), time);


       return UNIQUE_RESPONSE_OK;
}


int
main (int argc, char *argv[])
{
  GtkWidget *IndicMapper;
  UniqueApp *unique_app = NULL;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  setlocale (LC_ALL, "");
  gtk_init (&argc, &argv);

  //add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
       unique_app = unique_app_new_with_commands ("org.gnome.MainMenu", NULL,
                                                  "new_instance", COMMAND_NEW_INSTANCE, NULL);

       if (unique_app_is_running (unique_app))
        {
               unique_app_send_message (unique_app, COMMAND_NEW_INSTANCE, NULL);
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

