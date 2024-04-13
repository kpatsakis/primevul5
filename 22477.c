g_socket_client_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  switch (prop_id)
    {
    case PROP_FAMILY:
      g_socket_client_set_family (client, g_value_get_enum (value));
      break;

    case PROP_TYPE:
      g_socket_client_set_socket_type (client, g_value_get_enum (value));
      break;

    case PROP_PROTOCOL:
      g_socket_client_set_protocol (client, g_value_get_enum (value));
      break;

    case PROP_LOCAL_ADDRESS:
      g_socket_client_set_local_address (client, g_value_get_object (value));
      break;

    case PROP_TIMEOUT:
      g_socket_client_set_timeout (client, g_value_get_uint (value));
      break;

    case PROP_ENABLE_PROXY:
      g_socket_client_set_enable_proxy (client, g_value_get_boolean (value));
      break;

    case PROP_TLS:
      g_socket_client_set_tls (client, g_value_get_boolean (value));
      break;

    case PROP_TLS_VALIDATION_FLAGS:
      g_socket_client_set_tls_validation_flags (client, g_value_get_flags (value));
      break;

    case PROP_PROXY_RESOLVER:
      g_socket_client_set_proxy_resolver (client, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}