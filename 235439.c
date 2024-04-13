static int file_ram_open(const char *path,
                         const char *region_name,
                         bool readonly,
                         bool *created,
                         Error **errp)
{
    char *filename;
    char *sanitized_name;
    char *c;
    int fd = -1;

    *created = false;
    for (;;) {
        fd = open(path, readonly ? O_RDONLY : O_RDWR);
        if (fd >= 0) {
            /* @path names an existing file, use it */
            break;
        }
        if (errno == ENOENT) {
            /* @path names a file that doesn't exist, create it */
            fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
            if (fd >= 0) {
                *created = true;
                break;
            }
        } else if (errno == EISDIR) {
            /* @path names a directory, create a file there */
            /* Make name safe to use with mkstemp by replacing '/' with '_'. */
            sanitized_name = g_strdup(region_name);
            for (c = sanitized_name; *c != '\0'; c++) {
                if (*c == '/') {
                    *c = '_';
                }
            }

            filename = g_strdup_printf("%s/qemu_back_mem.%s.XXXXXX", path,
                                       sanitized_name);
            g_free(sanitized_name);

            fd = mkstemp(filename);
            if (fd >= 0) {
                unlink(filename);
                g_free(filename);
                break;
            }
            g_free(filename);
        }
        if (errno != EEXIST && errno != EINTR) {
            error_setg_errno(errp, errno,
                             "can't open backing store %s for guest RAM",
                             path);
            return -1;
        }
        /*
         * Try again on EINTR and EEXIST.  The latter happens when
         * something else creates the file between our two open().
         */
    }

    return fd;
}