
GtkWidget* create_IndicMapper (int argc, char **argv);
GtkWidget* create_IndicMapper_aboutdialog (void);

#define _BYTE   8
#define _WORD   16
#define _DWORD  32

enum {
        TARGET_STRING,
        TARGET_ROOTWIN
};

/* datatype (string), restrictions on DnD (GtkTargetFlags), datatype (int) */
static GtkTargetEntry target_list[] = {
        { "STRING",     0, TARGET_STRING },
        { "text/plain", 0, TARGET_STRING },
        { "application/x-rootwindow-drop", 0, TARGET_ROOTWIN }
};

void set_default_map_name(char *);
static guint n_targets = G_N_ELEMENTS (target_list);

