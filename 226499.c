lr_check_repomd_xml_asc_availability(LrHandle *handle,
                                     LrYumRepo *repo,
                                     int fd,
                                     char *path,
                                     GError **err)
{
    GError *tmp_err = NULL;
    gboolean ret;

    if (handle->checks & LR_CHECK_GPG) {
        int fd_sig;
        char *url, *signature;

        signature = lr_pathconcat(handle->destdir, "repodata/repomd.xml.asc", NULL);
        fd_sig = open(signature, O_CREAT | O_TRUNC | O_RDWR, 0666);
        if (fd_sig == -1) {
            g_debug("%s: Cannot open: %s", __func__, signature);
            g_set_error(err, LR_YUM_ERROR, LRE_IO,
                        "Cannot open %s: %s", signature, g_strerror(errno));
            lr_free(signature);
            return FALSE;
        }

        url = lr_pathconcat(handle->used_mirror, "repodata/repomd.xml.asc", NULL);
        ret = lr_download_url(handle, url, fd_sig, &tmp_err);
        lr_free(url);
        close(fd_sig);
        if (!ret) {
            // Error downloading signature
            g_set_error(err, LR_YUM_ERROR, LRE_BADGPG,
                        "GPG verification is enabled, but GPG signature "
                        "is not available. This may be an error or the "
                        "repository does not support GPG verification: %s", tmp_err->message);
            g_clear_error(&tmp_err);
            unlink(signature);
            lr_free(signature);
            return FALSE;
        } else {
            // Signature downloaded
            repo->signature = g_strdup(signature);
            ret = lr_gpg_check_signature(signature,
                                         path,
                                         handle->gnupghomedir,
                                         &tmp_err);
            lr_free(signature);
            if (!ret) {
                g_debug("%s: GPG signature verification failed: %s",
                        __func__, tmp_err->message);
                g_propagate_prefixed_error(err, tmp_err,
                                           "repomd.xml GPG signature verification error: ");
                return FALSE;
            }
            g_debug("%s: GPG signature successfully verified", __func__);
        }
    }

    return TRUE;
}