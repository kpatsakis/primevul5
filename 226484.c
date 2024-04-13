error_handling(GSList *targets, GError **dest_error, GError *src_error)
{
    if (src_error) {
        g_propagate_prefixed_error(dest_error, src_error,
                                   "Downloading error: ");
        return FALSE;
    } else {
        int code = LRE_OK;
        char *error_summary = NULL;

        for (GSList *elem = targets; elem; elem = g_slist_next(elem)) {
            LrDownloadTarget *target = elem->data;
            if (target->rcode != LRE_OK) {
                if (code == LRE_OK) {
                    // First failed download target found
                    code = target->rcode;
                    error_summary = g_strconcat(target->path,
                                                " - ",
                                                target->err,
                                                NULL);
                } else {
                    error_summary = g_strconcat(error_summary,
                                                "; ",
                                                target->path,
                                                " - ",
                                                target->err,
                                                NULL);
                }
            }

            close(target->fd);
        }

        if (code != LRE_OK) {
            // At least one target failed
            g_set_error(dest_error, LR_DOWNLOADER_ERROR, code,
                        "Downloading error(s): %s", error_summary);
            g_free(error_summary);
            return FALSE;
        }
    }

    return TRUE;
}