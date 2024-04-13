lr_yum_download_repo(LrHandle *handle,
                     LrYumRepo *repo,
                     LrYumRepoMd *repomd,
                     GError **err)
{
    gboolean ret = TRUE;
    GSList *targets = NULL;
    GSList *cbdata_list = NULL;
    GError *tmp_err = NULL;

    assert(!err || *err == NULL);

    prepare_repo_download_targets(handle, repo, repomd, NULL, &targets, &cbdata_list, err);

    if (!targets)
        return TRUE;

    ret = lr_download_single_cb(targets,
                                FALSE,
                                (cbdata_list) ? progresscb : NULL,
                                (cbdata_list) ? hmfcb : NULL,
                                &tmp_err);

    assert((ret && !tmp_err) || (!ret && tmp_err));
    ret = error_handling(targets, err, tmp_err);

    g_slist_free_full(cbdata_list, (GDestroyNotify)cbdata_free);
    g_slist_free_full(targets, (GDestroyNotify)lr_downloadtarget_free);

    return ret;
}