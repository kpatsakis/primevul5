SPICE_GNUC_VISIBLE int spice_server_add_ssl_client(SpiceServer *s, int socket, int skip_auth)
{
    RedLinkInfo *link;

    spice_assert(reds == s);
    if (!(link = reds_init_client_ssl_connection(socket))) {
        return -1;
    }

    link->skip_auth = skip_auth;
    return 0;
}
