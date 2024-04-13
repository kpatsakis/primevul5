lr_store_mirrorlist_files(LrHandle *handle,
                          LrYumRepo *repo,
                          GError **err)
{
    int fd;
    int rc;

    if (handle->mirrorlist_fd != -1) {
        char *ml_file_path = lr_pathconcat(handle->destdir,
                                           "mirrorlist", NULL);
        fd = open(ml_file_path, O_CREAT|O_TRUNC|O_RDWR, 0666);
        if (fd < 0) {
            g_debug("%s: Cannot create: %s", __func__, ml_file_path);
            g_set_error(err, LR_YUM_ERROR, LRE_IO,
                        "Cannot create %s: %s", ml_file_path, g_strerror(errno));
            lr_free(ml_file_path);
            return FALSE;
        }
        rc = lr_copy_content(handle->mirrorlist_fd, fd);
        close(fd);
        if (rc != 0) {
            g_debug("%s: Cannot copy content of mirrorlist file", __func__);
            g_set_error(err, LR_YUM_ERROR, LRE_IO,
                        "Cannot copy content of mirrorlist file %s: %s",
                        ml_file_path, g_strerror(errno));
            lr_free(ml_file_path);
            return FALSE;
        }
        repo->mirrorlist = ml_file_path;
    }

    return TRUE;
}