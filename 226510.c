lr_yum_download_remote(LrHandle *handle, LrResult *result, GError **err)
{
    gboolean ret = TRUE;
    int fd;
    LrYumRepo *repo;
    LrYumRepoMd *repomd;
    GError *tmp_err = NULL;

    assert(!err || *err == NULL);

    repo   = result->yum_repo;
    repomd = result->yum_repomd;

    g_debug("%s: Downloading/Copying repo..", __func__);

    if (!lr_prepare_repodata_dir(handle, err))
        return FALSE;

    if (!handle->update) {
        char *path = NULL;

        if (!lr_store_mirrorlist_files(handle, repo, err))
            return FALSE;

        if (!lr_copy_metalink_content(handle, repo, err))
            return FALSE;

        if ((fd = lr_prepare_repomd_xml_file(handle, &path, err)) == -1)
            return FALSE;

        /* Download repomd.xml */
        ret = lr_yum_download_repomd(handle, handle->metalink, fd, err);
        if (!ret) {
            close(fd);
            lr_free(path);
            return FALSE;
        }

        if (!lr_check_repomd_xml_asc_availability(handle, repo, fd, path, err)) {
            close(fd);
            lr_free(path);
            return FALSE;
        }

        lseek(fd, 0, SEEK_SET);

        /* Parse repomd */
        g_debug("%s: Parsing repomd.xml", __func__);
        ret = lr_yum_repomd_parse_file(repomd, fd, lr_xml_parser_warning_logger,
                                       "Repomd xml parser", &tmp_err);
        close(fd);
        if (!ret) {
            g_debug("%s: Parsing unsuccessful: %s", __func__, tmp_err->message);
            g_propagate_prefixed_error(err, tmp_err,
                                       "repomd.xml parser error: ");
            lr_free(path);
            return FALSE;
        }

        /* Fill result object */
        result->destdir = g_strdup(handle->destdir);
        repo->destdir = g_strdup(handle->destdir);
        repo->repomd = path;
        if (handle->used_mirror)
            repo->url = g_strdup(handle->used_mirror);
        else
            repo->url = g_strdup(handle->urls[0]);

        g_debug("%s: Repomd revision: %s", repomd->revision, __func__);
    }

    /* Download rest of metadata files */
    ret = lr_yum_download_repo(handle, repo, repomd, &tmp_err);
    assert((ret && !tmp_err) || (!ret && tmp_err));

    if (!ret) {
        g_debug("%s: Repository download error: %s", __func__, tmp_err->message);
        g_propagate_prefixed_error(err, tmp_err, "Yum repo downloading error: ");
        return FALSE;
    }

    return TRUE;
}