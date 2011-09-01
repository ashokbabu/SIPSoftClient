#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkstyle.h>
#include <libosso.h>
#include <hildon/hildon.h>
#include <hildon/hildon-banner.h>
#include <hildon/hildon-program.h>

#include <sys/time.h>
#include <time.h>

#include <libgnomevfs/gnome-vfs.h>
#include <gconf/gconf-client.h>

#include "phoneguiinterface.h"
#include <tp_caller.h>

#include <gst/gst.h>
#include <glib.h>


#define CONFIG_FILE_PATH "/usr/share/telepathy/sipphone.config"
#define IMAGE_DIR "/usr/share/telepathy/"
#define NO_OF_ENTRY_SETTINGS 4
#define BUFFER_SIZE 300
#define NO_OF_DIALPAD_BUTTONS 12

#define STAR_BUTTON_VALUE 10
#define HASH_BUTTON_VALUE 12





extern TpConn *action_login(const char* sip_address, const char* sip_password, const char* sip_proxy);
extern TpConn *conn;
extern int action_make_call(const char* to_uri);

extern void answer (char* channel_path, guint handle);
extern void set_output_volume_mute_sipclient(gboolean stat);
extern void set_output_volume_level(guint volume_level);

extern void set_sipclient_mic_input_volume_mute(gboolean mute_status);
				
static char redialExtension[15]="";
GstElement *play;

GtkWidget *entryDialText;
GtkWidget *backButton;
GtkWidget *dialButton,*holdButton,*hangupButton;
GtkWidget *redialButton;


gboolean clearDialTextFlag=FALSE; 

GtkWidget *fixed,*loginImage;

HildonVVolumebar *volume_control_bar;

GtkWidget *micButton,*miconImage,*micoffImage;
static gboolean micFlag=TRUE;

GtkWidget *username;
GtkWidget *loggedStatusImage;

GtkWidget* create_main_window (void)
{
  GtkWidget *main_window, *menubar1,*menuitem1,*menuitem1_menu1;
  GtkWidget *separatormenuitem1,*quit,*menuitem4,*settings;
  GtkWidget *menuitem4_menu,*about;

  
  gint x, y, c=0;
  gint tempx;
  
  GtkAccelGroup *accel_group = gtk_accel_group_new ();
  GdkColor color = {0, 0xcc00, 0xec00, 0xff00};


  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (main_window), "MSIP");
  

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (main_window), vbox);
  
  
  
 

  menubar1 = gtk_menu_bar_new ();
  menuitem1 = gtk_menu_item_new_with_mnemonic ("_File");
  menuitem1_menu1 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menuitem1_menu1);
   
  /*new = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);*/
	
 settings = gtk_menu_item_new_with_mnemonic ("_Settings");
 

  
  separatormenuitem1 = gtk_separator_menu_item_new ();
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);
  quit = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  menuitem4 = gtk_menu_item_new_with_mnemonic ("_Help");
  menuitem4_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem4), menuitem4_menu);
  about = gtk_menu_item_new_with_mnemonic ("_About");

  gtk_box_pack_start (GTK_BOX (vbox), menubar1, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu1), settings);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu1), separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu1), quit);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem4);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), about);



  entryDialText = (GtkWidget *)gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (entryDialText), 25);

  //GtkBorder bord = {4,4,4,4};  

  //gtk_entry_set_inner_border  ( entryDialText,&bord);
  
  gtk_entry_set_text (GTK_ENTRY(entryDialText), "");
  
/*  g_signal_connect (G_OBJECT (entryDialText), "activate",
		 G_CALLBACK (enter_callback),
		 (gpointer) entry);  This to be called in in dial button
 */
  
/*  gtk_box_pack_start (GTK_BOX (vbox), entryDialText, TRUE, TRUE, 0);*/
  
  

  alignment = gtk_alignment_new (0.4, 0.4, 0.4, 0.4);
  gtk_box_pack_start (GTK_BOX (vbox), alignment, TRUE, TRUE, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 20, 20, 40, 40);
     
 
  
  dialpad_table = gtk_table_new (6, 6, TRUE);
  gtk_container_add (GTK_CONTAINER (alignment), dialpad_table);
/*   gtk_widget_modify_bg(main_window, GTK_STATE_NORMAL, &color);*/
  
  
  for (x=0; x<NO_OF_DIALPAD_BUTTONS; x++) {	  
	  if(x<9){
		  gchar *temp = g_strdup_printf ("%i", x+1);
		  buttons[x] = gtk_button_new_with_mnemonic (temp);
		  g_free (temp);
	  } else if(x==(STAR_BUTTON_VALUE-1)){

		  gchar *temp = g_strdup_printf ("*");  
		  buttons[x] = gtk_button_new_with_mnemonic (temp);       
		  g_free (temp);
	  } else if(x==10){

		  gchar *temp = g_strdup_printf ("0");  
		  buttons[x] = gtk_button_new_with_mnemonic (temp);       
		  g_free (temp);

	  } else if(x==(HASH_BUTTON_VALUE)-1){	  
		 
		  gchar *temp = g_strdup_printf ("#");  
		  buttons[x] = gtk_button_new_with_mnemonic (temp);       
		  g_free (temp);		  
	  }
	 
#if 1
	 tempx = x+1; 
	  gtk_object_set_data (GTK_OBJECT(buttons[x]), "button_value", (gpointer) tempx);
 	  
	  if(x==10)
	  {
		  tempx = 0;
		  gtk_object_set_data (GTK_OBJECT(buttons[x]), "button_value", (gpointer) tempx);
	  }

#endif
	  
	 /* GdkColor dark = {0,0x6900,0xc600,0xff00};
	  GdkColor dark = {0,0x6911,0xc620,0xff00};
	  */
#if 0
	  GdkColor dark = {10,0x6911,0xc620,0xff00};
	  GtkStyle * style = gtk_style_copy(buttons[x]->style);
	  style->bg[0] = dark;
	  style->bg[1] = dark;
	  style->bg[2] = dark;
	  style->bg[3] = dark;
	  style->bg[4] = dark;
	  
	  gtk_widget_set_style(buttons[x], style);

#endif

  }


 gtk_table_attach(GTK_TABLE(dialpad_table), entryDialText, 1,4,0,1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 1, 1); 
 
  for (x=0; x<4; x++)
  	  for (y=0; y<3; y++)
			  gtk_table_attach (GTK_TABLE (dialpad_table), 
	  		  buttons[c++], y+1,y+2,x+1,x+2,
			  (GtkAttachOptions) GTK_FILL | GTK_EXPAND,
      			  (GtkAttachOptions) GTK_FILL | GTK_EXPAND,
      			  (GtkAttachOptions) 1, 1);


  

 gchar *temp = g_strdup_printf ("clear");
 backButton = gtk_button_new_with_mnemonic (temp);
 gtk_table_attach (GTK_TABLE (dialpad_table),backButton, 
		   4,5,0,1,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
  		   (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);
 
 
 temp = g_strdup_printf ("");
 dialButton = gtk_button_new_with_mnemonic (temp);

 GtkWidget *dialbuttonImage,*hangupbuttonImage,*holdbuttonImage,*redialbuttonImage;
 
 char file_name[50];
 sprintf(file_name,"%sdial.png",IMAGE_DIR);
 dialbuttonImage = gtk_image_new_from_file (file_name);
 gtk_button_set_image (dialButton,dialbuttonImage);
 gtk_widget_show(dialbuttonImage);
 gtk_widget_set_sensitive(dialButton,FALSE);
		    

 
						 
 /* gtk_image_set_from_pixbuf ((GtkImage*)dialButton, create_pixbuf("sipphone.png"));*/
 gtk_table_attach (GTK_TABLE (dialpad_table),dialButton, 
		   0,1,1,3,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
  		   (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);


#if 1
 
  volume_control_bar = HILDON_VVOLUMEBAR (hildon_vvolumebar_new ());
	gtk_widget_set_size_request (GTK_WIDGET (volume_control_bar), -1, 300);
	/* gtk_widget_show(bar); */
	gint dat;
	g_signal_connect (G_OBJECT (volume_control_bar), "mute_toggled", G_CALLBACK (mute_toggle), dat);
	g_signal_connect (G_OBJECT (volume_control_bar), "level_changed", G_CALLBACK (level_change), NULL);
	gtk_table_attach (GTK_TABLE (dialpad_table),volume_control_bar,
									5,6,0,6,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
									(GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);
	
#endif



		 

 
#if 1
 GtkWidget *logoimage;
 sprintf(file_name,"%smarslogo.png",IMAGE_DIR);
 logoimage = gtk_image_new_from_file (file_name);
 gtk_table_attach (GTK_TABLE (dialpad_table),logoimage,
		 0,1,0,1,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
		 (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);
 gtk_widget_show(logoimage);
     

#endif
 
sprintf(file_name,"%sloggedoff.png",IMAGE_DIR);
loggedStatusImage = gtk_image_new_from_file (file_name);
gtk_table_attach (GTK_TABLE (dialpad_table),loggedStatusImage,
								1,2,5,6,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
								(GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);
gtk_widget_show(loggedStatusImage);


char usercap[60];
sprintf(usercap,"User Name is : %s",read_from_file("Extension"));
username = gtk_label_new(usercap);
gtk_table_attach (GTK_TABLE (dialpad_table),username,
								1,5,5,6,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
								(GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);


 

 temp = g_strdup_printf ("");
 redialButton = gtk_button_new_with_mnemonic (temp);
 char fname[50];
 sprintf(fname,"%sredial.png",IMAGE_DIR);
 redialbuttonImage = gtk_image_new_from_file (fname);
 gtk_button_set_image (redialButton,redialbuttonImage);
 gtk_widget_show(redialbuttonImage);
 
 gtk_table_attach (GTK_TABLE (dialpad_table),redialButton,
		 4,5,1,3,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
		 (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);

 if(strcmp(redialExtension,"")==0)
	 gtk_widget_set_sensitive(redialButton,FALSE);
 
 
 
 holdButton = gtk_button_new();
 gtk_button_set_label(holdButton,"Hold");

 
 
/*  char fname[50];*/
#if  0
 sprintf(fname,"%scallhold.png",IMAGE_DIR);
 holdbuttonImage = gtk_image_new_from_file (fname);
 gtk_button_set_image (holdButton,holdbuttonImage);
 gtk_widget_show(holdbuttonImage);
#endif
       
 gtk_table_attach (GTK_TABLE (dialpad_table),holdButton, 
		   4,5,3,5,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
  		   (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);

 
 
 
 temp = g_strdup_printf ("");
 hangupButton = gtk_button_new_with_mnemonic (temp);
/* char fname[50];*/
 sprintf(fname,"%shang.png",IMAGE_DIR);
 hangupbuttonImage = gtk_image_new_from_file (fname); 
 gtk_button_set_image (hangupButton,hangupbuttonImage);
 gtk_widget_show(hangupbuttonImage);
 gtk_widget_set_sensitive(hangupButton,FALSE);
 gtk_table_attach (GTK_TABLE (dialpad_table),hangupButton, 
		   0,1,3,5,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
  		   (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);
 



 temp = g_strdup_printf ("");
 micButton = gtk_button_new_with_mnemonic (temp); 
 sprintf(fname,"%smicOn.png",IMAGE_DIR);
 miconImage = gtk_image_new_from_file (fname);
 gtk_button_set_image (micButton,miconImage);
 gtk_widget_show(miconImage);
 gtk_table_attach (GTK_TABLE (dialpad_table),micButton,
								 0,1,5,6,(GtkAttachOptions) GTK_FILL | GTK_EXPAND,
								 (GtkAttachOptions) GTK_FILL | GTK_EXPAND,(GtkAttachOptions) 1, 1);     						 



 g_free (temp);



 
  
  for (x=0; x<12; x++){
	
	  /*gtk_signal_connect(GTK_OBJECT(buttons[x]), "clicked", G_CALLBACK(on_digits_button_click), (gpointer)(x+1));*/
	  gtk_signal_connect(GTK_OBJECT(buttons[x]), "clicked", G_CALLBACK(on_digits_button_click), (gpointer)(entryDialText));
	
  }
 

  g_signal_connect ((gpointer) main_window, "delete_event", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect ((gpointer) settings, "activate", G_CALLBACK (on_settings_activate), NULL);
  g_signal_connect ((gpointer) quit, "activate", G_CALLBACK (on_quit_confirm_dialog), NULL); 
  g_signal_connect ((gpointer) about, "activate", G_CALLBACK (on_about_activate), NULL);



  gtk_signal_connect(GTK_OBJECT(backButton), "clicked", G_CALLBACK(on_back_button_click), (gpointer)(entryDialText));
  
  gtk_signal_connect(GTK_OBJECT(dialButton), "clicked", G_CALLBACK(on_dial_button_click), (gpointer)(entryDialText));
  gtk_signal_connect(GTK_OBJECT(holdButton), "clicked", G_CALLBACK(on_hold_button_click), (gpointer)(entryDialText));
  gtk_signal_connect(GTK_OBJECT(hangupButton), "clicked", G_CALLBACK(on_hangup_button_click), NULL);
  gtk_signal_connect(GTK_OBJECT(redialButton), "clicked", G_CALLBACK(on_redial_button_click), (gpointer)(entryDialText));	

	gtk_signal_connect(GTK_OBJECT(micButton), "clicked", G_CALLBACK(on_mic_button_click), NULL);
  
  gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);
  gtk_widget_show_all(main_window);
  on_startup_login();
   gtk_widget_set_sensitive(backButton,FALSE);

  return main_window;
  
}


void mute_toggle(){

				if(hildon_volumebar_get_mute(volume_control_bar)){

								g_print("\nMute bool is TRUE Before ");
								
								set_output_volume_mute_sipclient(TRUE);								
								g_print("\nMute bool is TRUE After");
				}
				else{
								g_print("\nMute bool is FALSE B4R");
								set_output_volume_mute_sipclient(FALSE); 
								g_print("\nMute bool is False After");

				}
								
}


void level_change(){

				guint volume_value= (int)hildon_volumebar_get_level(HILDON_VOLUMEBAR(volume_control_bar));
		    g_print("\n Set Volume levelTo :%i",volume_value);
				set_output_volume_level(volume_value);
}

		
/*
 * Action taken when Number Buttons are clicked 
 */ 

void on_digits_button_click(GtkWidget *button,gpointer data,GtkWidget *entryDialText) {
	
	char buffer[BUFFER_SIZE];
	gtk_widget_set_sensitive(dialButton,TRUE);
	gtk_widget_set_sensitive(backButton,TRUE);
	int dial_Button_Value= (gint)gtk_object_get_data(GTK_OBJECT(button),"button_value");
	
	if(dial_Button_Value == STAR_BUTTON_VALUE)
		sprintf(buffer,"%s","*");
	else if(dial_Button_Value == HASH_BUTTON_VALUE)
		sprintf(buffer,"%s","#");
	else
		sprintf(buffer,"%d",dial_Button_Value);
	
	const gchar *entry_text=(gchar*)buffer;
/*	gtk_entry_append_text(GTK_ENTRY(data), entry_text);*/
	
	if(clearDialTextFlag)
		gtk_entry_set_text(GTK_ENTRY(data),entry_text );
	else
		gtk_entry_append_text(GTK_ENTRY(data), entry_text);
	
	clearDialTextFlag=FALSE;

	
}

void on_back_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data) {

	const gchar *dialed_number;
	dialed_number = gtk_entry_get_text (GTK_ENTRY (entryDialText));

	char *buffer;
	buffer = (char *)malloc(30);
	sprintf(buffer,"%s",dialed_number);
	gtk_entry_set_text(GTK_ENTRY(entryDialText),remove_last_char_from_string(buffer) );	
	if(!strcmp(buffer,"")){
		gtk_widget_set_sensitive(backButton,FALSE);
		gtk_widget_set_sensitive(dialButton,FALSE);
	}
	
}


/*
 * Action taken when Dial button is clicked 
 */ 

void on_dial_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data) {
	gtk_widget_set_sensitive(redialButton,TRUE);
	gtk_widget_set_sensitive(dialButton,FALSE);
	gtk_widget_set_sensitive(backButton,FALSE);
	gtk_widget_set_sensitive(hangupButton,TRUE);
	const gchar *dialed_number;	
	dialed_number = gtk_entry_get_text (GTK_ENTRY (entryDialText));	
	strcpy(redialExtension,dialed_number);
	char sipuri[50];
	char buff[50];
	clearDialTextFlag=TRUE;
	sprintf(sipuri,"sip:%s@%s",dialed_number,read_from_file("IPAddress"));
	
	action_make_call(sipuri);
	sprintf(buff,"%s Calling....",dialed_number);
	show_status_label_text(buff);

}

/*
 * Action taken when Re-Dial button is clicked 
 */ 

void on_redial_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data) {

	g_print("Redial Button Pressed");
	char sipuri[50];
	char buff[50];
	const gchar *dialed_number;
	clearDialTextFlag=TRUE;
	gtk_widget_set_sensitive(redialButton,FALSE);
	gtk_widget_set_sensitive(redialButton,TRUE);
	dialed_number = redialExtension;
	sprintf(sipuri,"sip:%s@%s",dialed_number,read_from_file("IPAddress"));
	action_make_call(sipuri);
	sprintf(buff,"%s Calling....",dialed_number);
	show_status_label_text(buff);			

}


void show_status_label_text(char *label){
	
	gtk_entry_set_text(GTK_ENTRY(entryDialText), label);
	
}



/*
 * Action taken when Hang button is clicked 
 */ 

void on_hangup_button_click(GtkWidget *button,gpointer user_data) {
	clearDialTextFlag=TRUE;
	/* gtk_widget_set_sensitive(dialButton,TRUE);*/
	close_channel();
	show_status_label_text("Call Ended");
	g_print("Closing Channel on hangup ");
	show_status_label_text("");
}




/*
 * Action taken when Hold button is clicked 
 */ 

void on_hold_button_click(GtkWidget *button,GtkWidget *entryDialText,gpointer user_data) {
#if 0
 	static gboolean put_on_hold = TRUE;
	clearDialTextFlag=TRUE;	
	show_status_label_text("On Hold");
	call_hold(put_on_hold);
	if(put_on_hold)
		put_on_hold = FALSE;
	else
		put_on_hold = TRUE;
#endif

	if(strcmp(gtk_button_get_label(button),"Hold")==0){
					clearDialTextFlag=TRUE;
					show_status_label_text("On Hold");
					call_hold(TRUE);
					gtk_button_set_label(button,"Unhold");
	}else if(strcmp(gtk_button_get_label(button),"Unhold")==0){
					call_hold(FALSE);
					show_status_label_text("Connected");
					gtk_button_set_label(button,"Hold");
	}
	
	
	
}

void on_settings_activate (GtkMenuItem *menuitem, gpointer user_data) {

	GtkWidget *dialog,*content_area;
	char value[20];

	gint savetoFile;
	
	GtkWidget *entryIPAddress, *entryPassword,*entryExtension, *entryLocalip;
		    
	GtkWidget  *labelIPAddress, *labelPassword,*labelExtension, *labelLocalip;
		   
	GtkWidget  *table;
	
	
	dialog = gtk_dialog_new_with_buttons ("Settings",
			GTK_WINDOW(main_window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK,
			GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL,
			GTK_RESPONSE_REJECT,
			NULL);

	/*content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	content_area = GTK_DIALOG(dialog)->action_area;*/	
	content_area = GTK_DIALOG(dialog)->vbox;	
	
	labelExtension = gtk_label_new("Extension");   
	entryExtension = gtk_entry_new();	
      	gtk_entry_set_text(GTK_ENTRY(entryExtension), read_from_file("Extension"));
	
	labelPassword = gtk_label_new("Password");   
	entryPassword = gtk_entry_new();	
	gtk_entry_set_visibility( GTK_ENTRY(entryPassword),FALSE );
      	gtk_entry_set_text(GTK_ENTRY(entryPassword), read_from_file("Password"));
	
	labelIPAddress = gtk_label_new("Proxy IP Address");   
	entryIPAddress = gtk_entry_new();	
    	gtk_entry_set_text(GTK_ENTRY(entryIPAddress), read_from_file("IPAddress"));


	labelLocalip = gtk_label_new("Local IP Address");
	entryLocalip = gtk_entry_new();       
	gtk_entry_set_text(GTK_ENTRY(entryLocalip), read_from_file("LocalIP"));
			
	
	table = gtk_table_new(4, 2, FALSE);
	
	gtk_table_attach(GTK_TABLE(table), labelExtension, 0, 1, 0, 1, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), entryExtension, 1, 2, 0, 1, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), labelPassword, 0, 1, 1, 2, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), entryPassword, 1, 2, 1, 2, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), labelIPAddress, 0, 1, 2, 3, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), entryIPAddress, 1, 2, 2, 3, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

	gtk_table_attach(GTK_TABLE(table), labelLocalip, 0, 1, 3, 4, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), entryLocalip, 1, 2, 3, 4, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
		

	
	gtk_container_add(GTK_CONTAINER(content_area), table);
	
	gtk_widget_show_all (dialog);
	

	char *kkey[NO_OF_ENTRY_SETTINGS]={"Extension","Password","IPAddress","LocalIP"};
	char *vval[NO_OF_ENTRY_SETTINGS]={
		gtk_entry_get_text (GTK_ENTRY (entryExtension)),
		gtk_entry_get_text (GTK_ENTRY (entryPassword)),
		gtk_entry_get_text (GTK_ENTRY (entryIPAddress)),
		gtk_entry_get_text (GTK_ENTRY (entryLocalip)),
		
	};
	
	savetoFile = gtk_dialog_run (GTK_DIALOG (dialog));
	
		
	if (savetoFile == GTK_RESPONSE_ACCEPT){
	
		if( (strcmp(read_from_file("Extension"),gtk_entry_get_text (GTK_ENTRY (entryExtension)) ) != 0 ) ||
				(strcmp(read_from_file("Password"),gtk_entry_get_text (GTK_ENTRY (entryExtension)) ) != 0 ) ||
				(strcmp(read_from_file("IPAddress"),gtk_entry_get_text (GTK_ENTRY (entryExtension)) ) != 0 ) || 
				(strcmp(read_from_file("LocalIP"),gtk_entry_get_text (GTK_ENTRY (entryExtension)) ) != 0 )   )
		{
						
		action_logout();	
		write_to_file(kkey,vval);	
		
		char uscap[60];
		sprintf(uscap,"User Name is : %s",gtk_entry_get_text (GTK_ENTRY (entryExtension)));
		gtk_label_set_text(username,uscap);
		
		on_startup_login();
		}
		
	}

		

	gtk_widget_destroy (dialog);
	
	
	
}

char *read_from_file(char *key){


	FILE *read = fopen ( CONFIG_FILE_PATH, "r" );
	char *result=NULL;
       	char *val=NULL;
       	val = (char *)malloc(40);

      	if ( read == NULL ){
      		fprintf(stderr, "Error opening txt\n");
      		 return NULL;
      	}
      	char line[BUFFER_SIZE];
      	while ( fgets ( line, sizeof line, read ) != NULL ) {
      		size_t i = strspn ( line, " \t\n\v" );
      		if ( line[i] == '#' )
	      		continue;
		
		result = strtok(line, "=" );
	
		if(strcmp(result, key) == 0){

			result = strtok( NULL, "=" );
			if(result != NULL)
				strcpy(val,result);
	
			*(val+strlen(val)-1)='\0';
			if(*val != NULL){
				printf("In main Value is: %s",val);
				break;
			}else
	      			strcpy(val,"0");			
		}  
		
	}
	
	fclose(read);	
	return val;
	
}



write_to_file(char *key[], char *value[]){

	char *retString;
	retString = (char*)malloc(200);
	int i=0;
	
	char line[BUFFER_SIZE];
	char temp[BUFFER_SIZE];

	*retString='\0';	
	FILE *in = fopen(CONFIG_FILE_PATH,"r");
	


		while(fgets(line,sizeof line ,in)!= NULL){
			for( i=0;i<NO_OF_ENTRY_SETTINGS;i++){
				if(strncmp(line, key[i], strlen(key[i]))==0)
				{
					strcpy(line, key[i]);
					strcat(line,"=");
					strcat(line, value[i]);    
					strcpy(temp,line);
					strcat(retString,temp);
					strcat(retString,"\n");
				}
			}
		}

	fclose(in);

	FILE *out = fopen(CONFIG_FILE_PATH,"wb");
	if(out){
		fwrite(retString,sizeof(char),strlen(retString),out);
		fflush(out);
		fclose(out);
	}

	free(retString);
	retString=NULL;
		
}



void on_about_activate (GtkMenuItem *menuitem, gpointer user_data) {

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(main_window),
                                        GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
							"<b>Simple SIPPHONE Client </b>\nAuthor: Ashok Babu Chiruvella\nCompany: http://mars-india.com\n");
	
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}


void on_quit_confirm_dialog (GtkMenuItem *menuitem, gpointer user_data) {

	GtkWidget *dialog;
	gint result;
	dialog = gtk_message_dialog_new (GTK_WINDOW (main_window),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"Are you Sure want to Quit the Application");
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
			GTK_STOCK_YES, GTK_RESPONSE_YES,
			GTK_STOCK_NO, GTK_RESPONSE_NO,
			NULL);
	
	result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	
	if (result == GTK_RESPONSE_YES)
		gtk_main_quit();
	
}


GdkPixbuf *create_pixbuf(const gchar * filename)
{
	   GdkPixbuf *pixbuf;
	   GError *error = NULL;
	   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	   if(!pixbuf) {
		   fprintf(stderr, "%s\n", error->message);
		   g_error_free(error);
	   }
	   
		       return pixbuf;
}

char *remove_last_char_from_string(char *string){

	int len;
	char* pos; 
	len=strlen(string); 
	pos=string+len-1; 
	*pos='\0'; 
	return string;
	
}




void on_startup_login(){

	char* sip_proxy = getenv("SIP_PROXY");
	GtkWidget *banner;

	char tempBuf[50];
	if(( strcmp(read_from_file("Extension"),"0")==0 ) || (strcmp(read_from_file("IPAddress"),"0")==0) ||
			(strcmp(read_from_file("Password"),"0")==0)  || (strcmp(read_from_file("LocalIP"),"0")==0)    ) {

		banner = hildon_banner_show_information_with_markup (main_window, NULL,"Settings Not Configured Properly");
	        hildon_banner_set_timeout (HILDON_BANNER (banner), 9000);
	}		
	else{
		banner = hildon_banner_show_information_with_markup (main_window, NULL,"Logging in...");
		hildon_banner_set_timeout (HILDON_BANNER (banner), 5000);
		sprintf(tempBuf,"%s@%s",read_from_file("Extension"),read_from_file("IPAddress"));
		conn = action_login(tempBuf,read_from_file("Password"), sip_proxy);
	
		if (conn) {
			g_message("Connnection is created");
			banner = hildon_banner_show_information_with_markup (main_window, NULL,"Registered Successfully With Proxy");
			hildon_banner_set_timeout (HILDON_BANNER (banner), 5000);

			char tempfile[50];
			sprintf(tempfile,"%sloggedin.png",IMAGE_DIR);
			gtk_image_set_from_file(loggedStatusImage,tempfile);

			
		} else{
			banner = hildon_banner_show_information_with_markup (main_window, NULL,"Unable to Register with Proxy Server");
			hildon_banner_set_timeout (HILDON_BANNER (banner), 9000);
		}
	}		

}



gboolean isFilePresent(const char *filename){

				FILE* fp = NULL;
				fp = fopen(filename, "r");
				if(fp != NULL)
				{
								fclose(fp);
								return TRUE;
				}
				fclose(fp);
				return FALSE;
				
}


GtkWidget *dialog;

void on_incoming_call_accept_reject_dialog(char *channel_path, guint handle){
				
			  if(isFilePresent(INCOMING_RINGTONE_PATH)){
								play_ringtone(INCOMING_RINGTONE_PATH);	
								g_print("\n Playing Incoming Audio File");
				}else 
								g_print("\n No Audio File present..No Incoming sound\n");
												
				
				gint result;
			
				dialog = gtk_message_dialog_new (GTK_WINDOW (main_window),
												GTK_DIALOG_MODAL,
												GTK_MESSAGE_QUESTION,
												GTK_BUTTONS_NONE,
												"Incoming Call from xxxx");
			
				gtk_dialog_add_buttons (GTK_DIALOG (dialog),
												"ACCEPT", GTK_RESPONSE_ACCEPT,
												"REJECT", GTK_RESPONSE_REJECT,
												NULL);
			
				result = gtk_dialog_run (GTK_DIALOG (dialog));
				if (result == GTK_RESPONSE_ACCEPT){
								answer(channel_path,handle);
								stop_playing_ringtone();
								show_status_label_text("Connected");
								g_print("Call Accepted");
								gtk_widget_set_sensitive(hangupButton,TRUE);
				}
				if(result == GTK_RESPONSE_REJECT){
								g_print("Call Rejectedd");
								reject(channel_path,handle);
								stop_playing_ringtone();
								show_status_label_text("");
				}
			
				gtk_widget_destroy (dialog);
  	
}


void play_ringtone(char *ringtonepath)
{
				gchar filepath[400];
				sprintf(filepath,"file://%s",ringtonepath);
				play = gst_element_factory_make ("playbin", "play");
				g_object_set (G_OBJECT (play), "uri", filepath, NULL);
				gst_element_set_state (play, GST_STATE_PLAYING);
}

void stop_playing_ringtone(){

				gst_element_set_state (play, GST_STATE_NULL);
				gst_object_unref (GST_OBJECT (play));

}

void on_mic_button_click() {
				if(micFlag){
								gtk_widget_hide(miconImage);
								char fname[60];
								sprintf(fname,"%smicMute.png",IMAGE_DIR);			
								micoffImage = gtk_image_new_from_file (fname);
								gtk_button_set_image (micButton,micoffImage);
								gtk_widget_show(micoffImage);
								
								set_sipclient_mic_input_volume_mute(TRUE);												
								
								micFlag=FALSE;
				}else{
								gtk_widget_hide(micoffImage);
								char fname[60];
								sprintf(fname,"%smicOn.png",IMAGE_DIR);
								miconImage = gtk_image_new_from_file (fname);
								gtk_button_set_image (micButton,miconImage);
								gtk_widget_show(miconImage);
								
								set_sipclient_mic_input_volume_mute(FALSE);
								
								micFlag=TRUE;
				}				
}

