int pidfile_write(pid_t pid)
{
    int fd;
    char buffer[32];
    ssize_t nwritten;

    if (g_pidfile) {
        fd = open(g_pidfile, O_WRONLY|O_CREAT|O_TRUNC|O_NOFOLLOW,
                  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    } else if (pidfilefd >= 0) {
        fd = pidfilefd;
        g_pidfile = fd_to_filename(pidfilefd);
        if (!g_pidfile)
            goto error;
    } else {
        return 0;
    }

    if (fd < 0) {
        logprintf(STDERR_FILENO, "Could not open pidfile %s : %s\n",
                  g_pidfile, strerror(errno));
        goto error;
    }

    if (snprintf(buffer, sizeof(buffer), "%d", pid) >= (int)sizeof(buffer)) {
        logprintf(STDERR_FILENO, "Could not write pid to buffer\n");
        goto error_close;
    }

    nwritten = write_full(fd, buffer, strlen(buffer));
    if (nwritten < 0 || nwritten != (ssize_t)strlen(buffer)) {
        logprintf(STDERR_FILENO, "Could not write to pidfile : %s\n",
                  strerror(errno));
        goto error_close;
    }

    close(fd);

    return 0;

error_close:
    if (fd != pidfilefd)
        close(fd);

error:
    return -1;
}