lr_prepare_repomd_xml_file(LrHandle *handle,
                           char **path,
                           GError **err)
{
    int fd;

    *path = lr_pathconcat(handle->destdir, "/repodata/repomd.xml", NULL);
    fd = open(*path, O_CREAT|O_TRUNC|O_RDWR, 0666);
    if (fd == -1) {
        g_set_error(err, LR_YUM_ERROR, LRE_IO,
                    "Cannot open %s: %s", *path, g_strerror(errno));
        lr_free(*path);
        return -1;
    }

    return fd;
}