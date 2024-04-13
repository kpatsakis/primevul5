static void vnc_client_cache_auth(VncState *client)
{
    if (!client->info) {
        return;
    }

    if (client->tls) {
        client->info->x509_dname =
            qcrypto_tls_session_get_peer_name(client->tls);
        client->info->has_x509_dname =
            client->info->x509_dname != NULL;
    }
#ifdef CONFIG_VNC_SASL
    if (client->sasl.conn &&
        client->sasl.username) {
        client->info->has_sasl_username = true;
        client->info->sasl_username = g_strdup(client->sasl.username);
    }
#endif
}