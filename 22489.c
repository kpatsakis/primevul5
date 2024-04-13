g_socket_client_connect (GSocketClient       *client,
			 GSocketConnectable  *connectable,
			 GCancellable        *cancellable,
			 GError             **error)
{
  GIOStream *connection = NULL;
  GSocketAddressEnumerator *enumerator = NULL;
  GError *last_error, *tmp_error;

  last_error = NULL;

  if (can_use_proxy (client))
    {
      enumerator = g_socket_connectable_proxy_enumerate (connectable);
      if (client->priv->proxy_resolver &&
          G_IS_PROXY_ADDRESS_ENUMERATOR (enumerator))
        {
          g_object_set (G_OBJECT (enumerator),
                        "proxy-resolver", client->priv->proxy_resolver,
                        NULL);
        }
    }
  else
    enumerator = g_socket_connectable_enumerate (connectable);

  while (connection == NULL)
    {
      GSocketAddress *address = NULL;
      gboolean application_proxy = FALSE;
      GSocket *socket;
      gboolean using_proxy;

      if (g_cancellable_is_cancelled (cancellable))
	{
	  g_clear_error (error);
	  g_cancellable_set_error_if_cancelled (cancellable, error);
	  break;
	}

      tmp_error = NULL;
      g_socket_client_emit_event (client, G_SOCKET_CLIENT_RESOLVING,
				  connectable, NULL);
      address = g_socket_address_enumerator_next (enumerator, cancellable,
	      					  &tmp_error);

      if (address == NULL)
	{
	  if (tmp_error)
	    {
	      g_clear_error (&last_error);
	      g_propagate_error (error, tmp_error);
	    }
	  else if (last_error)
	    {
	      g_propagate_error (error, last_error);
	    }
	  else
            g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                                 _("Unknown error on connect"));
	  break;
	}
      g_socket_client_emit_event (client, G_SOCKET_CLIENT_RESOLVED,
				  connectable, NULL);

      using_proxy = (G_IS_PROXY_ADDRESS (address) &&
		     client->priv->enable_proxy);

      /* clear error from previous attempt */
      g_clear_error (&last_error);

      socket = create_socket (client, address, &last_error);
      if (socket == NULL)
	{
	  g_object_unref (address);
	  continue;
	}

      connection = (GIOStream *)g_socket_connection_factory_create_connection (socket);
      g_socket_connection_set_cached_remote_address ((GSocketConnection*)connection, address);
      g_socket_client_emit_event (client, G_SOCKET_CLIENT_CONNECTING, connectable, connection);

      if (g_socket_connection_connect (G_SOCKET_CONNECTION (connection),
				       address, cancellable, &last_error))
	{
          g_socket_connection_set_cached_remote_address ((GSocketConnection*)connection, NULL);
	  g_socket_client_emit_event (client, G_SOCKET_CLIENT_CONNECTED, connectable, connection);
	}
      else
	{
	  clarify_connect_error (last_error, connectable, address);
	  g_object_unref (connection);
	  connection = NULL;
	}

      if (connection && using_proxy)
	{
	  GProxyAddress *proxy_addr = G_PROXY_ADDRESS (address);
	  const gchar *protocol;
	  GProxy *proxy;

	  protocol = g_proxy_address_get_protocol (proxy_addr);

          /* The connection should not be anything else then TCP Connection,
           * but let's put a safety guard in case
	   */
          if (!G_IS_TCP_CONNECTION (connection))
            {
              g_critical ("Trying to proxy over non-TCP connection, this is "
                          "most likely a bug in GLib IO library.");

              g_set_error_literal (&last_error,
                  G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                  _("Proxying over a non-TCP connection is not supported."));

	      g_object_unref (connection);
	      connection = NULL;
            }
	  else if (g_hash_table_contains (client->priv->app_proxies, protocol))
	    {
	      application_proxy = TRUE;
	    }
          else if ((proxy = g_proxy_get_default_for_protocol (protocol)))
	    {
	      GIOStream *proxy_connection;

	      g_socket_client_emit_event (client, G_SOCKET_CLIENT_PROXY_NEGOTIATING, connectable, connection);
	      proxy_connection = g_proxy_connect (proxy,
						  connection,
						  proxy_addr,
						  cancellable,
						  &last_error);
	      g_object_unref (connection);
	      connection = proxy_connection;
	      g_object_unref (proxy);

	      if (connection)
		g_socket_client_emit_event (client, G_SOCKET_CLIENT_PROXY_NEGOTIATED, connectable, connection);
	    }
	  else
	    {
	      g_set_error (&last_error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
			   _("Proxy protocol “%s” is not supported."),
			   protocol);
	      g_object_unref (connection);
	      connection = NULL;
	    }
	}

      if (!application_proxy && connection && client->priv->tls)
	{
	  GIOStream *tlsconn;

	  tlsconn = g_tls_client_connection_new (connection, connectable, &last_error);
	  g_object_unref (connection);
	  connection = tlsconn;

	  if (tlsconn)
	    {
	      g_tls_client_connection_set_validation_flags (G_TLS_CLIENT_CONNECTION (tlsconn),
                                                            client->priv->tls_validation_flags);
	      g_socket_client_emit_event (client, G_SOCKET_CLIENT_TLS_HANDSHAKING, connectable, connection);
	      if (g_tls_connection_handshake (G_TLS_CONNECTION (tlsconn),
					      cancellable, &last_error))
		{
		  g_socket_client_emit_event (client, G_SOCKET_CLIENT_TLS_HANDSHAKED, connectable, connection);
		}
	      else
		{
		  g_object_unref (tlsconn);
		  connection = NULL;
		}
	    }
	}

      if (connection && !G_IS_SOCKET_CONNECTION (connection))
	{
	  GSocketConnection *wrapper_connection;

	  wrapper_connection = g_tcp_wrapper_connection_new (connection, socket);
	  g_object_unref (connection);
	  connection = (GIOStream *)wrapper_connection;
	}

      g_object_unref (socket);
      g_object_unref (address);
    }
  g_object_unref (enumerator);

  g_socket_client_emit_event (client, G_SOCKET_CLIENT_COMPLETE, connectable, connection);
  return G_SOCKET_CONNECTION (connection);
}