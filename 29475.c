static void reds_accept(int fd, int event, void *data)
{
    int socket;

    if ((socket = accept(reds->listen_socket, NULL, 0)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return;
    }

    if (spice_server_add_client(reds, socket, 0) < 0)
        close(socket);
}
