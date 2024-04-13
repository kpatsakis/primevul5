prepare_repo_download_std_target(LrHandle *handle,
                                 LrYumRepoMdRecord *record,
                                 char **path,
                                 int *fd,
                                 GSList **checksums,
                                 GSList **targets,
                                 GError **err)
{
    *path = lr_pathconcat(handle->destdir, record->location_href, NULL);
    *fd = open(*path, O_CREAT|O_TRUNC|O_RDWR, 0666);
    if (*fd < 0) {
        g_debug("%s: Cannot create/open %s (%s)",
                __func__, *path, g_strerror(errno));
        g_set_error(err, LR_YUM_ERROR, LRE_IO,
                    "Cannot create/open %s: %s", *path, g_strerror(errno));
        lr_free(*path);
        g_slist_free_full(*targets, (GDestroyNotify) lr_downloadtarget_free);
        return FALSE;
    }

    if (handle->checks & LR_CHECK_CHECKSUM) {
        // Select proper checksum type only if checksum check is enabled
        LrDownloadTargetChecksum *checksum;
        checksum = lr_downloadtargetchecksum_new(
                       lr_checksum_type(record->checksum_type),
                       record->checksum);
        *checksums = g_slist_prepend(*checksums, checksum);
    }
    return TRUE;
}