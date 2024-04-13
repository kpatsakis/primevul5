lr_yum_download_url(LrHandle *lr_handle, const char *url, int fd,
                    gboolean no_cache, gboolean is_zchunk, GError **err)
{
    gboolean ret;
    LrDownloadTarget *target;
    GError *tmp_err = NULL;

    assert(url);
    assert(!err || *err == NULL);

    CbData *cbdata = cbdata_new(lr_handle->user_data,
                                NULL,
                                lr_handle->user_cb,
                                lr_handle->hmfcb,
                                url);

    // Prepare target
    target = lr_downloadtarget_new(lr_handle,
                                   url, NULL, fd, NULL,
                                   NULL, 0, 0,(lr_handle->user_cb) ? progresscb : NULL, cbdata,
                                   NULL, (lr_handle->hmfcb) ? hmfcb : NULL, NULL, 0, 0,
                                   NULL, no_cache, is_zchunk);

    // Download the target
    ret = lr_download_target(target, &tmp_err);

    assert(ret || tmp_err);
    assert(!(target->err) || !ret);
    cbdata_free(cbdata);

    if (!ret)
        g_propagate_error(err, tmp_err);

    lr_downloadtarget_free(target);

    lseek(fd, 0, SEEK_SET);

    return ret;
}