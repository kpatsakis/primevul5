int server_open_native_socket(Server*s) {
        union sockaddr_union sa;
        int one, r;
        struct epoll_event ev;

        assert(s);

        if (s->native_fd < 0) {

                s->native_fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
                if (s->native_fd < 0) {
                        log_error("socket() failed: %m");
                        return -errno;
                }

                zero(sa);
                sa.un.sun_family = AF_UNIX;
                strncpy(sa.un.sun_path, "/run/systemd/journal/socket", sizeof(sa.un.sun_path));

                unlink(sa.un.sun_path);

                r = bind(s->native_fd, &sa.sa, offsetof(union sockaddr_union, un.sun_path) + strlen(sa.un.sun_path));
                if (r < 0) {
                        log_error("bind() failed: %m");
                        return -errno;
                }

                chmod(sa.un.sun_path, 0666);
        } else
                fd_nonblock(s->native_fd, 1);

        one = 1;
        r = setsockopt(s->native_fd, SOL_SOCKET, SO_PASSCRED, &one, sizeof(one));
        if (r < 0) {
                log_error("SO_PASSCRED failed: %m");
                return -errno;
        }

#ifdef HAVE_SELINUX
        one = 1;
        r = setsockopt(s->syslog_fd, SOL_SOCKET, SO_PASSSEC, &one, sizeof(one));
        if (r < 0)
                log_warning("SO_PASSSEC failed: %m");
#endif

        one = 1;
        r = setsockopt(s->native_fd, SOL_SOCKET, SO_TIMESTAMP, &one, sizeof(one));
        if (r < 0) {
                log_error("SO_TIMESTAMP failed: %m");
                return -errno;
        }

        zero(ev);
        ev.events = EPOLLIN;
        ev.data.fd = s->native_fd;
        if (epoll_ctl(s->epoll_fd, EPOLL_CTL_ADD, s->native_fd, &ev) < 0) {
                log_error("Failed to add native server fd to epoll object: %m");
                return -errno;
        }

        return 0;
}
