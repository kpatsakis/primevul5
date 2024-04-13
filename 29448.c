void server_process_native_file(
                Server *s,
                int fd,
                struct ucred *ucred,
                struct timeval *tv,
                const char *label, size_t label_len) {

        struct stat st;
        void *p;
        ssize_t n;

        assert(s);
        assert(fd >= 0);

        /* Data is in the passed file, since it didn't fit in a
         * datagram. We can't map the file here, since clients might
         * then truncate it and trigger a SIGBUS for us. So let's
         * stupidly read it */

        if (fstat(fd, &st) < 0) {
                log_error("Failed to stat passed file, ignoring: %m");
                return;
        }

        if (!S_ISREG(st.st_mode)) {
                log_error("File passed is not regular. Ignoring.");
                return;
        }

        if (st.st_size <= 0)
                return;

        if (st.st_size > ENTRY_SIZE_MAX) {
                log_error("File passed too large. Ignoring.");
                return;
        }

        p = malloc(st.st_size);
        if (!p) {
                log_oom();
                return;
        }

        n = pread(fd, p, st.st_size, 0);
        if (n < 0)
                log_error("Failed to read file, ignoring: %s", strerror(-n));
        else if (n > 0)
                server_process_native_message(s, p, n, ucred, tv, label, label_len);

        free(p);
}
