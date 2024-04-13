lr_prepare_repodata_dir(LrHandle *handle,
                        GError **err)
{
    int rc;
    int create_repodata_dir = 1;
    char *path_to_repodata;

    path_to_repodata = lr_pathconcat(handle->destdir, "repodata", NULL);

    if (handle->update) {  /* Check if should create repodata/ subdir */
        struct stat buf;
        if (stat(path_to_repodata, &buf) != -1)
            if (S_ISDIR(buf.st_mode))
                create_repodata_dir = 0;
    }

    if (create_repodata_dir) {
        /* Prepare repodata/ subdir */
        rc = mkdir(path_to_repodata, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
        if (rc == -1) {
            g_debug("%s: Cannot create dir: %s (%s)",
                    __func__, path_to_repodata, g_strerror(errno));
            g_set_error(err, LR_YUM_ERROR, LRE_CANNOTCREATEDIR,
                        "Cannot create directory: %s: %s",
                        path_to_repodata, g_strerror(errno));
            lr_free(path_to_repodata);
            return FALSE;
        }
    }
    lr_free(path_to_repodata);

    return TRUE;
}