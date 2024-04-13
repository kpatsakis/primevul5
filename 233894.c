static socket_t bind_socket(ssh_bind sshbind, const char *hostname,
    int port) {
    char port_c[6];
    struct addrinfo *ai;
    struct addrinfo hints;
    int opt = 1;
    socket_t s;
    int rc;

    ZERO_STRUCT(hints);

    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_c, 6, "%d", port);
    rc = getaddrinfo(hostname, port_c, &hints, &ai);
    if (rc != 0) {
        ssh_set_error(sshbind,
                      SSH_FATAL,
                      "Resolving %s: %s", hostname, gai_strerror(rc));
        return -1;
    }

    s = socket (ai->ai_family,
                           ai->ai_socktype,
                           ai->ai_protocol);
    if (s == SSH_INVALID_SOCKET) {
        ssh_set_error(sshbind, SSH_FATAL, "%s", strerror(errno));
        freeaddrinfo (ai);
        return -1;
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&opt, sizeof(opt)) < 0) {
        ssh_set_error(sshbind,
                      SSH_FATAL,
                      "Setting socket options failed: %s",
                      strerror(errno));
        freeaddrinfo (ai);
        close(s);
        return -1;
    }

    if (bind(s, ai->ai_addr, ai->ai_addrlen) != 0) {
        ssh_set_error(sshbind,
                      SSH_FATAL,
                      "Binding to %s:%d: %s",
                      hostname,
                      port,
                      strerror(errno));
        freeaddrinfo (ai);
        close(s);
        return -1;
    }

    freeaddrinfo (ai);
    return s;
}