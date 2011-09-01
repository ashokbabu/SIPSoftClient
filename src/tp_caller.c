
#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

#include <tp_caller.h>

extern void show_status_label_text(char *label);
extern void on_incoming_call_accept_reject_dialog(char *channel_path, guint handle);

extern void play_ringtone();
extern void stop_playing_ringtone();

static gboolean global_connected = FALSE;
static guint global_remote_handle = 0;
/*static*/ GMainLoop *global_mainloop_sp = NULL;
static DBusGProxy *global_streamengine = NULL;
/*static*/ DBusGConnection *global_dbus_connection = NULL;
static gint  global_exit_request = 0;
static char* global_conn_path = NULL;
static char* global_chan_path = NULL;
static guint global_chan_handle = 0;
static guint global_call_state = STATE_START;

static gboolean isIncomingCallStream=FALSE;
static gboolean isOutgoingCallStream=FALSE;
TpConn *conn;

TpChan *channel = NULL;

GPtrArray *ashstreamids;

extern char* read_from_file(char *);

DBusGProxy *ashokMediaStream = NULL;


void close_channel_cb(DBusGProxy *proxy, GError *error, gpointer userdata)
{
				show_status_label_text("Disconnected");
				stop_playing_ringtone();
				g_print("\n.Dial Ring Back Tone stopped");	
				printf("\nClosing the Channel...............Callback\n");
				show_status_label_text("");
}

void close_channel()
{
        tp_chan_close_async(DBUS_G_PROXY(channel),
	                    close_channel_cb, NULL);
}

void action_logout()
{
	GError *error = NULL;

       tp_conn_disconnect (DBUS_G_PROXY(conn), &error);
       if (error)
           g_error_free (error);
	       
}
	
TpConn *action_login(const char* sip_address, const char* sip_password, const char* sip_proxy)
{
  DBusGConnection *connection;
  //TpConn *conn;
  DBusGProxy *streamengine = NULL;
  TpConnMgr *connmgr;
  guint status = 1;
  GError *error = NULL;
  GValue val_acc, val_auth_usr, val_pwd, val_proxy, val_local_ip, val_discover_stun;
  GHashTable *connection_parameters = g_hash_table_new(g_str_hash, NULL);
  
  /* const char *local_eth_ip = getenv("LOCAL_ETH_ADDRESS");*/
  
  const char *local_eth_ip = read_from_file("LocalIP");

  connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
  if (connection == NULL) {
    g_printerr("Failed to open connection to bus: %s\n",
	       error->message);
    g_error_free(error);
    exit(1);
  }

  printf("connected to DBus with connection %p\n", connection);
  global_dbus_connection = connection;

  /* Create a connection manager object */
  g_print("Attempting to create a connection manager object.\n");
  connmgr = tp_connmgr_new(connection, CONNMGR_BUS, CONNMGR_PATH,
			   TP_IFACE_CONN_MGR_INTERFACE);

  if (connmgr == NULL) {
    g_error("Failed to create a connection manager, skipping manager startup.");
  }
  else {
    g_print("Creating a connection manager succeeded.\n");
  }

  g_print("Attempting to register a signal handler for NewConnection signal.\n");
  dbus_g_proxy_connect_signal(DBUS_G_PROXY(connmgr), "NewConnection",
			      G_CALLBACK(newconnection_handler),
			      NULL, NULL);
	
  /* Setting "g_type" is a hack since GValue is broken */
  val_acc.g_type = 0;
  val_auth_usr.g_type = 0;
  val_proxy.g_type = 0;
  val_pwd.g_type = 0;
  val_local_ip.g_type = 0;

  g_value_init(&val_acc, G_TYPE_STRING);
  g_value_init(&val_auth_usr, G_TYPE_STRING);
  g_value_init(&val_proxy, G_TYPE_STRING);
  g_value_init(&val_pwd, G_TYPE_STRING);
  g_value_init(&val_local_ip, G_TYPE_STRING);
  //g_value_init(&val_discover_stun, G_TYPE_BOOLEAN);

  /* Connection parameters are dummy: fill in suitable replacements */

  g_value_set_string(&val_acc, sip_address);
  g_value_set_string(&val_pwd, sip_password);
  if(local_eth_ip)
  {
	   g_print("IP Addres from ENV = %s\n",local_eth_ip);
  	g_value_set_string(&val_local_ip, local_eth_ip);
  }
  else
  {
	   g_print("IP Addres = 172.16.25.40\n");
	g_value_set_string(&val_local_ip, "172.16.25.40");
  }
  //g_value_set_boolean(&val_discover_stun, FALSE);
	
  g_hash_table_insert(connection_parameters, "account",
		      (gpointer) &val_acc);
  g_hash_table_insert(connection_parameters, "password",
		      (gpointer) &val_pwd);
  g_hash_table_insert(connection_parameters, "local-ip-address",
		      (gpointer) &val_local_ip);
  //g_hash_table_insert(connection_parameters, "discover-stun",
//		      (gpointer) &val_discover_stun);
  if (sip_proxy != NULL) {
    g_value_set_string(&val_proxy, sip_proxy);
    g_hash_table_insert(connection_parameters, "proxy-host",
			(gpointer) &val_proxy);  
  }

  /* Create a new actual connection with the connection manager */

  g_print("Attempting to create a connection object.\n");

  conn = tp_connmgr_new_connection(connmgr,
				   connection_parameters,
				   PROTOCOL);
  g_assert(conn != NULL);

  /* step: connection creation succesful */
  dbus_g_proxy_connect_signal(DBUS_G_PROXY(conn), "NewChannel",
			      G_CALLBACK(new_channel_handler),
			      NULL, NULL);
  g_print("Creation of a connection object succeeded.\n");

  streamengine = dbus_g_proxy_new_for_name (connection,
					    "org.freedesktop.Telepathy.StreamEngine",
					    "/org/freedesktop/Telepathy/StreamEngine",
					    "org.freedesktop.Telepathy.ChannelHandler");
/*
  streamengine = dbus_g_proxy_new_for_name (connection,
					    "org.freedesktop.Telepathy.VoipEngine",
					    "/org/freedesktop/Telepathy/VoipEngine",
					    "org.freedesktop.Telepathy.ChannelHandler");
*/
  g_assert(streamengine != NULL);
  global_streamengine = streamengine;

  /* Check if connection is active; if not, add a callback
   * for StatusChange signal */

  if (!tp_conn_get_status(DBUS_G_PROXY(conn), &status, &error) || status != 0) {
    g_print("GetStatus did not work synchronously, trying async version.\n");

    dbus_g_proxy_connect_signal(DBUS_G_PROXY(conn), "StatusChanged",
				G_CALLBACK(status_changed_cb),
				NULL, NULL);
    if (error) 
      g_error_free(error);

    while (global_connected != TRUE && global_exit_request == 0)  {
      g_main_context_iteration(NULL, TRUE);
    }
  } else {
    check_conn_properties(conn);
  }

  g_hash_table_destroy(connection_parameters);

  return conn;
}


int action_make_call(/*TpConn *conn,*/ const char* to_uri)
{
  DBusGProxy *streamengine = global_streamengine;
  GError *error = NULL;
  //TpChan *channel;
  DBusGProxy *stream = NULL;
  DBusGProxy *interface = NULL;
  GArray *types = g_array_sized_new (FALSE, FALSE, sizeof (guint), 1);
  guint mediatype = TP_MEDIA_STREAM_TYPE_AUDIO;
  const char *urilist[2] = { NULL, NULL };
  int result = 0;
  guint old_state = STATE_START;

  channel =NULL;
  urilist[0] = to_uri;

  if(conn == NULL)
  {
	  g_print("User is not logged in Connection is NULL\n");
	  return 0;
  }

  /* state machine for setting up a call */

  while (global_call_state != STATE_COMPLETED) {

    switch (global_call_state) 
      {
      case STATE_START:
	g_print("Requesting handle for SIP URI %s.\n", to_uri);
	tp_conn_request_handles_async(DBUS_G_PROXY(conn), TP_CONN_HANDLE_TYPE_CONTACT, (const char**)urilist, request_handles_reply_cb, NULL);

	break;

      case STATE_CHAN:

	/* request for a channel for outbound call  */
	g_print("Attempting to make an outbound call to %s (%d).\n", to_uri, global_remote_handle);
	tp_conn_request_channel_async(DBUS_G_PROXY(conn),
				      TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA,
				      TP_CONN_HANDLE_TYPE_CONTACT, global_remote_handle, TRUE,
				      request_chan_cb, NULL);
	
	break;

      case STATE_STREAMS:
	g_debug("Calling HandleChannel on %p, connection='%s', chan='%s'.\n",
		streamengine, global_conn_path, global_chan_path);

	error = NULL;
	
	tp_ch_handle_channel (streamengine, 
			      CONNMGR_BUS,
			      global_conn_path, 
			      TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA, 
			      global_chan_path, 
			      TP_CONN_HANDLE_TYPE_CONTACT, 
			      global_remote_handle, 
			      &error);

	if (error) {
	  g_print ("ERROR: %s", error->message);
	  g_error_free (error);
	  g_object_unref (streamengine);
	  result = -1;

	}
	else 
	  g_print ("Succesful HandleChannel with streamengine.\n");

	channel = tp_chan_new (global_dbus_connection,
			       CONNMGR_BUS,
			       global_chan_path,
			       TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA,
			       TP_CONN_HANDLE_TYPE_CONTACT,
			       global_remote_handle);
 
	g_assert (channel);
	
	 dbus_g_proxy_connect_signal(DBUS_G_PROXY(channel), "Closed",
			               G_CALLBACK(close_channel_cb),
				       NULL, NULL);
	 
/*	 dbus_g_proxy_connect_signal(DBUS_G_PROXY(channel), "channel-error",
			               G_CALLBACK(new_channelerror_handler),
				       NULL, NULL);
	 */

	stream = tp_chan_get_interface (channel,
					TELEPATHY_CHAN_IFACE_STREAMED_QUARK);
	ashokMediaStream = stream;
	if (stream) {
	 dbus_g_proxy_connect_signal(DBUS_G_PROXY(stream), "StreamStateChanged",
			               G_CALLBACK(new_channelonchannel_handler),
				       NULL, NULL);
	  g_array_append_val (types, mediatype);
    
	  g_debug("%s: calling RequestStream with types->len=%d", G_STRFUNC, types->len);
    
	  tp_chan_type_streamed_media_request_streams_async (stream,
							     global_remote_handle,
							     /*global_chan_handle ,*/
							     types,
							     request_streams_cb,
							     NULL);
	}
#if 0
	interface = tp_chan_get_interface (channel,TP_IFACE_QUARK_CHANNEL_INTERFACE_CALL_STATE);
	if(interface)
	{
		g_debug("Call State Interface created\n");
/*	dbus_g_proxy_connect_signal(DBUS_G_PROXY(interface), "CallStateChanged",
			             G_CALLBACK(new_channelerror_handler),
				     NULL, NULL);
*/	}
	else
		 g_debug("Call State Interface ERROR taqi\n");
#endif
	break;
	
      case STATE_RUNNING: 
	/* stream ready, call setup completed */
	global_call_state = STATE_COMPLETED;
	isOutgoingCallStream=TRUE;
#if 1
	
	interface = tp_chan_get_interface (channel,TP_IFACE_QUARK_CHANNEL_INTERFACE_CALL_STATE);
	if(interface)
	{
		g_debug("Call State Interface created\n");
	dbus_g_proxy_connect_signal(DBUS_G_PROXY(interface), "CallStateChanged",
			             G_CALLBACK(call_state_change_handler),
									 NULL,NULL);
	//			     global_chan_handle, global_call_state);
	}
	else 
		g_debug("Call State Interface ERROR taqi\n");

#endif
#if 0
 g_print("\n before my call");	
if(!tp_chan_iface_call_state_get_call_states(DBUS_G_PROXY(channel),G_CALLBACK(my_call_state_change_handler),NULL))
{
				g_print("\n failed in getting call states");
}
 g_print("\n After my call");	

#endif
 
	//g_debug("TAQI CALL STATE RUNNING......... Calling hold\n");
	//tp_chan_iface_hold_request_hold_async(DBUS_G_PROXY(channel), FALSE, hold_callback, NULL );
	break;
      }

    /* run the mainloop */
    while (global_call_state == old_state && 
	   global_exit_request == 0) {
      g_main_context_iteration(NULL, TRUE);
    }

    old_state = global_call_state;

    if (global_exit_request)
      global_call_state = STATE_COMPLETED;

  }
	global_call_state = STATE_START;



  return result;
}


void newconnection_handler(DBusGProxy *proxy, const char *bus,
				  const char *path, const char *proto,
				  gpointer user_data)

{
  g_print("NewConnection callback:\n\tbus=%s\n\tpath=%s\n\tproto=%s\n", bus, path, proto);
  global_conn_path = g_strdup(path);
}



void  display_incoming_number(DBusGProxy *proxy,
								guint contact_handle,
								gchar *string){
				
				g_print("\n Incoming number --------contactid=%u && string = %s",contact_handle,string);
				

}

void handle_incoming_call (DBusGConnection *proxy, const char *chan_path, guint handle, gboolean doaccept)
{

	g_print("\n Incoming Call parameters \n channelPath=%s \n handle=%i\n",chan_path,handle);

  GError *error = NULL;
  GArray *array;
  //TpChan *channel_in;
  channel=NULL;
  DBusGProxy *chgroup = NULL;
  
  channel = tp_chan_new (proxy,
			 CONNMGR_BUS,
			 chan_path,
			 TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA,
			 TP_CONN_HANDLE_TYPE_CONTACT,
			 handle);
  g_assert (channel);
#if 1
	
/* DBusGProxy *ininterface;
 ininterface = tp_chan_get_interface (channel,TELEPATHY_CHAN_IFACE_CONTACTLIST_QUARK);

if(ininterface) 

	g_print("\n~~~~~~~~~~~~~~~~before signal \n");
 dbus_g_proxy_connect_signal(DBUS_G_PROXY(channel), "GotContactInfo",
			               G_CALLBACK(display_incoming_number),
				       NULL, NULL);
	g_print("\n~~~~~~~~~~~~~~~~After signal \n");
 
*/
 
#endif 
      
  chgroup = tp_chan_get_interface (channel,
				   TELEPATHY_CHAN_IFACE_GROUP_QUARK);
  g_assert (chgroup);

	 dbus_g_proxy_connect_signal(DBUS_G_PROXY(channel), "Closed",
			               G_CALLBACK(close_channel_cb),
				       NULL, NULL);
  g_print("\tInbound call, passing the media channel to stream engine.\n");

  /* step 1: pass the channel to the stream engine */
      
  tp_ch_handle_channel (global_streamengine, 
			CONNMGR_BUS,
			global_conn_path, 
			TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA, 
			chan_path, 
			TP_CONN_HANDLE_TYPE_CONTACT, 
			handle, 
			&error);

  if (error) {
    g_print ("ERROR: %s", error->message);
    g_error_free (error);
  }

  /* step 2: inform connection manager we accept the session */

  tp_chan_iface_group_get_self_handle (chgroup, &global_chan_handle, &error);
  if (error) {
    g_warning ("cannot get self handle: %s", error->message);
    g_error_free (error), error = NULL;
  }

  g_print("\tAccepting the call (self handle %d).\n", global_chan_handle);

  array = g_array_new (FALSE, FALSE, sizeof (guint));
  g_array_append_val (array, global_chan_handle);

  if(doaccept == TRUE)
  {
  	printf("Accepting the Call\n");
  	tp_chan_iface_group_add_members (chgroup, array, "", &error);
		isIncomingCallStream=TRUE;
  	if (error) {
    	g_warning ("cannot add member %u to media channel: %s", handle, error->message);
	    g_error_free (error), error = NULL;
  	}
  }
  else
  {
	  printf("Rejecting the Call\n");
  	tp_chan_iface_group_remove_members (chgroup, array, "", &error);
		isIncomingCallStream=FALSE;
  	if (error) {
    	g_warning ("cannot remove member %u to media channel: %s", handle, error->message);
    	g_error_free (error), error = NULL;
  	}
  }
  g_array_free(array, TRUE);
}

void new_channel_handler(DBusGProxy *proxy, const char *object_path,
				const char *channel_type, guint handle_type,
				guint handle, gboolean suppress_handler,
				gpointer user_data)
{
	GPtrArray** chans;
	GError** error;
	gpointer *temp;
  g_print("Ashok-NewChannel callback\n\t path=%s\n\ttype=%s, handle=%u\n handle_type=%u\n",
	  object_path, channel_type, handle,handle_type);
  
	if (strcmp(channel_type, TP_IFACE_CHANNEL_TYPE_STREAMED_MEDIA) == 0) {
    global_chan_path = g_strdup(object_path);
    /* note: handle == 0 for inbound calls */
    global_chan_handle = handle;
		
#if 0
		GHashTable *callstatemap=g_hash_table_new(g_str_hash,g_str_equal);
	  GError** hasherror;
#endif
		
    if (suppress_handler == 0) {

#if 0
		tp_chan_iface_call_state_get_call_states(proxy,&callstatemap,&hasherror);
		g_hash_table_foreach(callstatemap,(GHFunc)print_data,NULL);
#endif
	g_print("\n~~~~~~~~~~~~~~~~before signal \n");
 dbus_g_proxy_connect_signal(DBUS_G_PROXY(proxy), "GotContactInfo",
			               G_CALLBACK(display_incoming_number),
				       NULL, NULL);
	g_print("\n~~~~~~~~~~~~~~~~After signal \n");
	    
	    on_incoming_call_accept_reject_dialog(global_chan_path, handle);
    }
		
  }
}


static void print_data(gchar *key,gchar *value){

				g_print("\n Key=%s   Value=%s\n",key,value);
				
}


void answer (char* channel_path, guint handle)
{
      handle_incoming_call (global_dbus_connection, global_chan_path, handle, TRUE);
}

void reject(char* channel_path, guint handle)
{
      handle_incoming_call (global_dbus_connection, global_chan_path, handle, FALSE);
	//close_channel();
}

//#if 0
void new_channelonchannel_handler(DBusGProxy *proxy, const char *object_path,
				const char *channel_type, guint handle_type,
				guint handle, gboolean suppress_handler,
				gpointer user_data)
{
	g_printf("********* stream state changed\n");
}

void my_call_state_change_handler(DBusGProxy *proxy, 
								GHashTable **out,
								GError **error	)
{
				
	g_printf("********* inside CALL STATE changed _handler\n");
	g_hash_table_foreach(out,(GHFunc)printdata,NULL);
}

printdata(gchar *key,gchar *value){
				g_print("\n...%s == %s \n",key,value);

				
}

void call_state_change_handler(DBusGProxy *proxy, 
								guint call_handle,
								 guint call_state)

{
	g_printf("********* inside CALL STATE changed _handler\n");

#define RINGING 1
#define CONNECTED 9
 if(call_state == RINGING){
				 if(isFilePresent(OUTGOING_RINGTONE_PATH)){
								 play_ringtone(OUTGOING_RINGTONE_PATH);
								 g_print("\n Playing Audio File");
 							   show_status_label_text("Ringing.....");
				 }else
								 g_print("Audio file not present..hence no outgoing ring sound");
 }
 else if(call_state == CONNECTED){
				 stop_playing_ringtone();
				 show_status_label_text("Connected..");
 }


}

void request_chan_cb(DBusGProxy *proxy,
			    gchar *channel_path,
			    GError *error,
			    gpointer data)
{

  g_print("RequestChan callback:\n\tchanpath=%s\n", channel_path);

  if (error != NULL) {
    g_warning("%s: error in callback - %s\n", G_STRFUNC, error->message);
    g_error_free(error);
    global_exit_request = 1;
    return;
  }
  
  if (channel_path == NULL) {
    global_exit_request = 1;
    return;
  }

  global_call_state = STATE_STREAMS;
}

void request_streams_cb(DBusGProxy *proxy,
			       GPtrArray *streams,
			       GError *error,
			       gpointer data)
{
	int i=0;
  g_print("RequestStreams callback with %d streams\n", streams ? streams->len : 0);
 ashstreamids = streams;

 for( i=0;i<streams->len;i++){
				g_print("\n...Ashok System- Stream Idsss are :%i\n",streams[i]);
 }
 
 for( i=0;i<streams->len;i++){
				g_print("\n...Ashok System- Stream Idsss are :%i\n",ashstreamids[i]);
 }

  if (error != NULL) {
    g_warning("%s: error in callback - %s\n", G_STRFUNC, error->message);
    g_error_free(error);
    return;
  }

  global_call_state = STATE_RUNNING;
}

gboolean status_changed_cb(DBusGProxy *proxy,
				  guint status, guint reason,
				  gpointer user_data)
{
				g_print("StatusChanged signal\n");
				if(status==0){
								g_print("\tConnected!\n");
								global_connected = TRUE;
								check_conn_properties(TELEPATHY_CONN(proxy));
				}
				else {
								global_connected = FALSE;
				}
				return TRUE;
}

void request_handles_reply_cb(DBusGProxy *proxy, GArray *handles, GError *error, gpointer userdata)
{
  guint i;

  g_print("RequestHandles callback\n");

  if (error) {
    g_warning("%s: error in callback - %s\n", G_STRFUNC, error->message);
    g_error_free(error);
    global_exit_request = 1;
    return;
  }
    
  for(i = 0; handles && i < handles->len; i++) {
    guint ret_handle = g_array_index(handles, guint, i);

    if (i == 0) {
      global_remote_handle = ret_handle;
    }

    g_print("\tRequested handle received: %u (item %u)\n", ret_handle, i);
  }

  global_call_state = STATE_CHAN;
}

void tpcaller_signal_handler(int signo)
{
  global_exit_request = 1;

  if (global_mainloop_sp)
    g_main_loop_quit(global_mainloop_sp);
}

void check_conn_properties(TpConn *conn)
{
  TpPropsIface *conn_props;

  conn_props = TELEPATHY_PROPS_IFACE (tp_conn_get_interface (
        conn, TELEPATHY_PROPS_IFACE_QUARK));
  if (conn_props == NULL) {
    g_warning ("The connection object does not support " TP_IFACE_PROPERTIES);  
  }
}

void set_sipclient_mic_input_volume_mute(gboolean mute_status){
				
				GError *error = NULL;
				gboolean retvalue;
				DBusGProxy *ashok_streamengine = NULL;
				if((isOutgoingCallStream == TRUE) || (isIncomingCallStream == TRUE)){
				ashok_streamengine = dbus_g_proxy_new_for_name (global_dbus_connection,
												"org.freedesktop.Telepathy.StreamEngine",
												"/org/freedesktop/Telepathy/StreamEngine",
												"org.freedesktop.Telepathy.StreamEngine");
				
				if(!dbus_g_proxy_call(DBUS_G_PROXY(ashok_streamengine),"MuteInput",&error,
																DBUS_TYPE_G_OBJECT_PATH, global_chan_path,
																G_TYPE_UINT,0,
																G_TYPE_BOOLEAN, mute_status,
																G_TYPE_INVALID,
																G_TYPE_BOOLEAN, &retvalue,
																G_TYPE_INVALID)
					){
								g_print("\n *********....Unable to do dbus_proxy_call..In Mic mute input....*******\n");
								g_print("\n Error is :%s",error->message);
				}else
									g_print("\n**********Able to do Dbus Proxy cal..mic mute l**");			

				if(retvalue)
								g_print("\n**Return Value is True mic**\n");
				else
								g_print("\n RValue is False mic\n");
				}else
								g_print("\n no stream to set input vol mute");
				
}
				

void set_output_volume_mute_sipclient(gboolean mute_status){
				GError *error = NULL;
				gboolean retvalue;
				DBusGProxy *ashok_streamengine = NULL;

				if((isOutgoingCallStream == TRUE) || (isIncomingCallStream == TRUE)){
				ashok_streamengine = dbus_g_proxy_new_for_name (global_dbus_connection,
												"org.freedesktop.Telepathy.StreamEngine",
												"/org/freedesktop/Telepathy/StreamEngine",
												"org.freedesktop.Telepathy.StreamEngine");
				
				
				if(!dbus_g_proxy_call(DBUS_G_PROXY(ashok_streamengine),"MuteOutput",&error,
																DBUS_TYPE_G_OBJECT_PATH, global_chan_path,
																G_TYPE_UINT,0,
																G_TYPE_BOOLEAN, mute_status,
																G_TYPE_INVALID,
																G_TYPE_BOOLEAN, &retvalue,
																G_TYPE_INVALID)
					){
								g_print("\n *********....Unable to do dbus_proxy_call......Ashok..*******\n");
								g_print("\n Error is :%s",error->message);
				}else
									g_print("\n**********Able to do Dbus Proxy call**");			

				if(retvalue)
								g_print("\n**Return Value is True **\n");
				else
								g_print("\n RValue is False\n");
				}else
								g_print("\n No Stream to Set output vol mute\n");
				
				
}


void set_output_volume_level(guint volume_level){
				GError *error = NULL;
				gboolean retvalue;
				DBusGProxy *ashok_streamengine = NULL;

				if((isOutgoingCallStream == TRUE) || (isIncomingCallStream == TRUE)){

								
				ashok_streamengine = dbus_g_proxy_new_for_name (global_dbus_connection,
																"org.freedesktop.Telepathy.StreamEngine",
																"/org/freedesktop/Telepathy/StreamEngine",
																"org.freedesktop.Telepathy.StreamEngine");
				
				
				if(!dbus_g_proxy_call(DBUS_G_PROXY(ashok_streamengine),"SetOutputVolume",&error,
																DBUS_TYPE_G_OBJECT_PATH, global_chan_path,
																G_TYPE_UINT,0,
																G_TYPE_UINT,volume_level,
																G_TYPE_INVALID,
																G_TYPE_BOOLEAN, &retvalue,
																G_TYPE_INVALID)
					){
								g_print("\n *********....Unable to do dbus_proxy_call......Ashok..*******\n");
								g_print("\n Error is :%s",error->message);
				}else
									g_print("\n**********Able to do Dbus Proxy call**");			

				if(retvalue)
								g_print("\n**Return Value is True **\n");
				else
								g_print("\n RValue is False\n");

				}else			
								g_print("\n No stream to change volume\n");
				

}



static void hold_callback(DBusGProxy *proxy,
			    GError *error,
	   		    gpointer data)
{
		printf("Hold Callback...................................................\n");
}

void call_hold(gboolean onhold)
{
	GError **err;
	if(channel)
	{
		DBusGProxy *interface = tp_chan_get_interface (channel,TP_IFACE_QUARK_CHANNEL_INTERFACE_HOLD);
		if(interface)
		{
		if(onhold)
			printf("Putting call on hold....\n");
		else
			printf("Unholding the call ....\n");

		tp_chan_iface_hold_request_hold_async(DBUS_G_PROXY(interface), onhold, hold_callback, NULL );
		}
//		tp_chan_iface_hold_request_hold(DBUS_G_PROXY(channel), onhold, err);
	}
	else
		printf("Channel is not active\n");
}
