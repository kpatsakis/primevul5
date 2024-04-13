g_socket_client_emit_event (GSocketClient       *client,
			    GSocketClientEvent  event,
			    GSocketConnectable  *connectable,
			    GIOStream           *connection)
{
  g_signal_emit (client, signals[EVENT], 0,
		 event, connectable, connection);
}