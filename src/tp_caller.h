#ifndef TP_CALLER_H
#define TP_CALLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <dbus/dbus-glib.h>
#include <glib.h>
#include <libtelepathy/tp-conn.h>
#include <libtelepathy/tp-connmgr.h>
#include <libtelepathy/tp-chan.h>
#include <libtelepathy/tp-chan-gen.h>
#include <libtelepathy/tp-ch-gen.h>
#include <libtelepathy/tp-chan-iface-group-gen.h>
#include <libtelepathy/tp-chan-type-text-gen.h>
#include <libtelepathy/tp-chan-type-streamed-media-gen.h>
#include <libtelepathy/tp-props-iface.h>
#include <libtelepathy/tp-constants.h>
#include <libtelepathy/tp-interfaces.h>
#include <libtelepathy/tp-chan-iface-hold-gen.h>


#include <libtelepathy/tp-chan-iface-call-state-gen.h>

#define CONNMGR_NAME "sofiasip"
#define CONNMGR_BUS "org.freedesktop.Telepathy.ConnectionManager.sofiasip"
#define CONNMGR_PATH "/org/freedesktop/Telepathy/ConnectionManager/sofiasip"
#define PROTOCOL "sip"


#define INCOMING_RINGTONE_PATH "/usr/share/telepathy/dialtone.wav"
#define OUTGOING_RINGTONE_PATH "/usr/share/telepathy/dialtone.wav"
#if 0
#define OUTGOING_RINGTONE_PATH "/usr/share/telepathy/ringbacktone.wav"
#endif

enum {
	STATE_START = 0,
	STATE_CHAN,
	STATE_STREAMS,
	STATE_RUNNING,
	STATE_COMPLETED,
};

#if 0
static gboolean global_connected = FALSE;
static guint global_remote_handle = 0;
/*static*/ GMainLoop *global_mainloop_sp = NULL;
static DBusGProxy *global_streamengine = NULL;
static DBusGConnection *global_dbus_connection = NULL;
static gint  global_exit_request = 0;
static char* global_conn_path = NULL;
static char* global_chan_path = NULL;
static guint global_chan_handle = 0;
static guint global_call_state = STATE_START;
#endif

void newconnection_handler(DBusGProxy *proxy, const char *s1,
								  const char *o, const char *s2,
								  gpointer user_data);

void new_channel_handler(DBusGProxy *proxy, const char *object_path,
								const char *channel_type, guint handle_type,
								guint handle, gboolean suppress_handle,
								gpointer user_data);

gboolean status_changed_cb(DBusGProxy *proxy,
								  guint status, guint reason,
								  gpointer user_data);

void request_handles_reply_cb(DBusGProxy *proxy, GArray *OUT_arg2, GError *error, gpointer userdata);

void request_chan_cb(DBusGProxy *proxy,
						 	gchar *channel_path,
						    GError *error,
						    gpointer data);

static void request_streams_cb(DBusGProxy *proxy,
						       GPtrArray *streams,
						       GError *error,
						       gpointer data);

//void tpcaller_signal_handler(int signo);

void check_conn_properties(TpConn *conn);

void new_channelonchannel_handler(DBusGProxy *proxy, const char *object_path,
		                                const char *channel_type, guint handle_type,
						                                guint handle, gboolean suppress_handler,
										                                gpointer user_data);

#if 0
void call_state_change_handler(DBusGProxy *proxy, const char *object_path,
                               const char *channel_type, guint handle_type,															 
                               gpointer/*guint*/ user_data);
#endif


void call_state_change_handler(DBusGProxy *proxy,
								guint call_handle,
								guint states);

void my_call_state_change_handler(DBusGProxy *proxy,
 								GHashTable **out,
								GError **error );
printdata(gchar *key,gchar *value);

void close_channel();
void answer (char* channel_path, guint handle);
void reject(char* channel_path, guint handle);

void action_logout();
static void hold_callback(DBusGProxy *proxy,
                          GError *error,
                          gpointer data);
void call_hold();
void print_data(gchar *key,gchar *value);
#if 0
/* The function should be called while starting the application */
TpConn *action_login(const char* sip_address, const char* sip_password, const char* sip_proxy);

/* The function makes a call to the to_uri 
 * Call this funtion whenever user presses the Dial button or 
 * entered the called number terminated by # key
 */
int action_make_call(TpConn *conn, const char* to_uri);
#endif
#endif
void  display_incoming_number(DBusGProxy *proxy,               
								guint contact_handle,
								gchar *string);
				
