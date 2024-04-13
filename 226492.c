lr_yum_repomd_record_enabled(LrHandle *handle, const char *type, GSList* records)
{
    // Blacklist check
    if (handle->yumblist) {
        int x = 0;
        while (handle->yumblist[x]) {
            if (!strcmp(handle->yumblist[x], type))
                return FALSE;
            x++;
        }
    }

    // Whitelist check
    if (handle->yumdlist) {
        int x = 0;
        while (handle->yumdlist[x]) {
            if (!strcmp(handle->yumdlist[x], type))
                return TRUE;
            x++;
        }
        // Substitution check
        if (handle->yumslist) {
            for (GSList *elem = handle->yumslist; elem; elem = g_slist_next(elem)) {
                LrVar* subs = elem->data;
                if (!g_strcmp0(subs->val, type)) {
                    char *orig = subs->var;
                    for (guint i = 0; handle->yumdlist[i]; i++) {
                        if (!g_strcmp0(orig, handle->yumdlist[i]) &&
                            !g_slist_find_custom(records, orig, (GCompareFunc) compare_records))
                            return TRUE;
                    }
                    return FALSE;
                }
            }
        }
        return FALSE;
    }
    return TRUE;
}