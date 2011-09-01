#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk/gtkstyle.h>
#include <libosso.h>

#include "phoneguiinterface.h"

#include <hildon/hildon-banner.h>
#include <hildon/hildon-program.h>

#include <sys/time.h>
#include <time.h>

#include <tp_caller.h>

extern GtkWidget* create_mainWindow (void);

extern void tpcaller_signal_handler(int signo);

/* The function should be called while starting the application 
extern TpConn *action_login(const char* sip_address, const char* sip_password, const char* sip_proxy);

*/


/* The function makes a call to the to_uri
   Call this funtion whenever user presses the Dial button or
   entered the called number terminated by # key
	 
	 //extern int action_make_call(TpConn *conn, const char* to_uri);
*/

extern GMainLoop *global_mainloop_sp;
extern DBusGConnection *global_dbus_connection;
extern TpConn *conn;

int main(int argc, char **argv)
{
//  TpConn *conn;
  GError *error = NULL;
  GMainLoop *mainloop;
  char* sip_proxy = getenv("SIP_PROXY");
#if 0
#ifndef _WIN32
  /* see: http://www.opengroup.org/onlinepubs/007908799/xsh/sigaction.html */
  struct sigaction sigact;
  memset(&sigact, 0, sizeof(sigact)); 
  sigact.sa_handler = tpcaller_signal_handler;
  sigaction(SIGINT, &sigact, NULL); /* ctrl-c */
  sigaction(SIGABRT, &sigact, NULL);
  sigaction(SIGTERM, &sigact, NULL);
#endif
#endif
#if 0    
  if (argc < 3) {
    g_message("Usage: tp_caller <sip-aor> <password> [<sip-address-to-call> <sip-outbound-proxy>]");
    exit(1);
  } 
#endif
  
#if 0    
  g_type_init();
  global_mainloop_sp = mainloop = g_main_loop_new (NULL, FALSE);
  if (sip_proxy == NULL)
    sip_proxy = argc > 4 ? argv[4] : NULL;
#endif
	osso_context_t* ctx = NULL;
	gst_init(&argc, &argv);
	gtk_init (&argc, &argv);
	GtkWidget *main_windowi,*banner;
	main_window = create_main_window ();

	ctx = osso_initialize("org.maemo.sipphone", "1.0", TRUE, NULL);
	if (ctx == NULL) {
					g_print("Failed to init LibOSSO\n");
					return 0;
	}
	g_print("LibOSSO Init done\n");
	gtk_widget_show_all (main_window);
	gtk_main ();
	
	g_print("Disconnecting from network.\n");
	tp_conn_disconnect (DBUS_G_PROXY(conn), &error);
	if (error)
				g_error_free (error);
	g_object_unref (conn);
	dbus_g_connection_unref (global_dbus_connection);
	g_print("Closing connection to SIP network.\n");

	return 0;

}
 	
