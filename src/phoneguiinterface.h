#ifndef __PHONEGUIINTERFACE_H__
#define __PHONEGUIINTERFACE_H__

#define CONFIG_FILE_DIR "/usr/share/pixmaps/"
#define BUF_SIZE 255

GtkWidget *undo_button;

char buffer[BUF_SIZE];

GtkWidget *main_window, *vbox;
GtkWidget *dialpad_table;
GtkWidget *buttons[12];
GtkWidget *statusbar;
GtkWidget *alignment;
GtkWidget *image;

GtkWidget* create_main_window (void);

void on_digits_button_click(GtkWidget *button,gpointer user_data,GtkWidget *entryDialText);
void on_back_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data);
void on_dial_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data);
void on_hangup_button_click(GtkWidget *button,gpointer user_data);
void on_hold_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data);
void on_settings_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_about_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_quit_confirm_dialog (GtkMenuItem *menuitem, gpointer user_data);
char* remove_last_char_from_string(char *string);

void settings_callback(GtkWidget *button,gpointer data);
GdkPixbuf *create_pixbuf(const gchar * filename);
void write_to_file(char **key,char **value);
char *read_from_file(char *key);
void show_information_note (GtkWidget *parent);
void on_startup_login(void);
void show_status_label_text(char *label);
GtkWidget *create_hildon_button (void);
void on_incoming_call_accept_reject_dialog(char *channel_path, guint handle);

void on_redial_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data);



void play_ringtone();
void stop_playing_ringtone();
void level_change();
void mute_toggle();

void on_mic_button_click();
		

#endif
