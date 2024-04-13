g_socket_client_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  switch (prop_id)
    {
      case PROP_FAMILY:
	g_value_set_enum (value, client->priv->family);
	break;

      case PROP_TYPE:
	g_value_set_enum (value, client->priv->type);
	break;

      case PROP_PROTOCOL:
	g_value_set_enum (value, client->priv->protocol);
	break;

      case PROP_LOCAL_ADDRESS:
	g_value_set_object (value, client->priv->local_address);
	break;

      case PROP_TIMEOUT:
	g_value_set_uint (value, client->priv->timeout);
	break;

      case PROP_ENABLE_PROXY:
	g_value_set_boolean (value, client->priv->enable_proxy);
	break;

      case PROP_TLS:
	g_value_set_boolean (value, g_socket_client_get_tls (client));
	break;

      case PROP_TLS_VALIDATION_FLAGS:
	g_value_set_flags (value, g_socket_client_get_tls_validation_flags (client));
	break;

      case PROP_PROXY_RESOLVER:
	g_value_set_object (value, g_socket_client_get_proxy_resolver (client));
	break;

      default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}