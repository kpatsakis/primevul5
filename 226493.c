lr_yum_download_repos(GSList *targets,
                      GError **err)
{
    gboolean ret;
    GSList *download_targets = NULL;
    GSList *cbdata_list = NULL;
    GError *download_error = NULL;

    for (GSList *elem = targets; elem; elem = g_slist_next(elem)) {
        LrMetadataTarget *target = elem->data;

        if (!target->handle) {
            continue;
        }

        prepare_repo_download_targets(target->handle,
                                      target->repo,
                                      target->repomd,
                                      target,
                                      &download_targets,
                                      &cbdata_list,
                                      &download_error);
    }

    if (!download_targets) {
        g_propagate_error(err, download_error);
        return TRUE;
    }

    ret = lr_download_single_cb(download_targets,
                                FALSE,
                                (cbdata_list) ? progresscb : NULL,
                                (cbdata_list) ? hmfcb : NULL,
                                &download_error);

    error_handling(download_targets, err, download_error);

    g_slist_free_full(cbdata_list, (GDestroyNotify)cbdata_free);
    g_slist_free_full(download_targets, (GDestroyNotify)lr_downloadtarget_free);

    return ret;
}