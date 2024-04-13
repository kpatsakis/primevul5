clarify_connect_error (GError             *error,
		       GSocketConnectable *connectable,
		       GSocketAddress     *address)
{
  const char *name;
  char *tmp_name = NULL;

  if (G_IS_PROXY_ADDRESS (address))
    {
      name = tmp_name = g_inet_address_to_string (g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (address)));

      g_prefix_error (&error, _("Could not connect to proxy server %s: "), name);
    }
  else
    {
      if (G_IS_NETWORK_ADDRESS (connectable))
	name = g_network_address_get_hostname (G_NETWORK_ADDRESS (connectable));
      else if (G_IS_NETWORK_SERVICE (connectable))
	name = g_network_service_get_domain (G_NETWORK_SERVICE (connectable));
      else if (G_IS_INET_SOCKET_ADDRESS (connectable))
	name = tmp_name = g_inet_address_to_string (g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (connectable)));
      else
	name = NULL;

      if (name)
	g_prefix_error (&error, _("Could not connect to %s: "), name);
      else
	g_prefix_error (&error, _("Could not connect: "));
    }

  g_free (tmp_name);
}