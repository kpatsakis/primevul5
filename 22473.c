g_socket_client_set_tls_validation_flags (GSocketClient        *client,
					  GTlsCertificateFlags  flags)
{
  if (client->priv->tls_validation_flags != flags)
    {
      client->priv->tls_validation_flags = flags;
      g_object_notify (G_OBJECT (client), "tls-validation-flags");
    }
}