lyxml_parse_path(struct ly_ctx *ctx, const char *filename, int options)
{
    FUN_IN;

    struct lyxml_elem *elem = NULL;
    size_t length;
    int fd;
    char *addr;

    if (!filename || !ctx) {
        LOGARG;
        return NULL;
    }

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        LOGERR(ctx, LY_EINVAL,"Opening file \"%s\" failed.", filename);
        return NULL;
    }
    if (lyp_mmap(ctx, fd, 0, &length, (void **)&addr)) {
        LOGERR(ctx, LY_ESYS, "Mapping file descriptor into memory failed (%s()).", __func__);
        goto error;
    } else if (!addr) {
        /* empty XML file */
        goto error;
    }

    elem = lyxml_parse_mem(ctx, addr, options);
    lyp_munmap(addr, length);
    close(fd);

    return elem;

error:
    if (fd != -1) {
        close(fd);
    }

    return NULL;
}