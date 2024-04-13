static void reds_accept_ssl_connection(int fd, int event, void *data)
{
    RedLinkInfo *link;
    int socket;

    if ((socket = accept(reds->secure_listen_socket, NULL, 0)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return;
    }

    if (!(link = reds_init_client_ssl_connection(socket))) {
        close(socket);
        return;
    }
}
