lr_yum_switch_to_zchunk(LrHandle *handle, LrYumRepoMd *repomd)
{
    if (handle->yumdlist) {
        int x = 0;
        while (handle->yumdlist[x]) {
            char *check_type = g_strconcat(handle->yumdlist[x], "_zck", NULL);
            assert(check_type);

            /* Check whether we already want the zchunk version of this record */
            int found = FALSE;
            int y = 0;
            while (handle->yumdlist[y]) {
                if (y == x) {
                    y++;
                    continue;
                }
                if (strcmp(handle->yumdlist[y], check_type) == 0) {
                    found = TRUE;
                    break;
                }
                y++;
            }
            if (found) {
                g_free(check_type);
                x++;
                continue;
            }

            found = FALSE;
            /* Check whether the zchunk version of this record exists */
            for (GSList *elem = repomd->records; elem; elem = g_slist_next(elem)) {
                LrYumRepoMdRecord *record = elem->data;

                if (strcmp(record->type, check_type) == 0) {
                    g_debug("Found %s so using instead of %s", check_type,
                            handle->yumdlist[x]);
                    g_free(handle->yumdlist[x]);
                    handle->yumdlist[x] = check_type;
                    found = TRUE;
                    break;
                }
            }
            if (!found)
                g_free(check_type);
            x++;
        }
    }
    return;
}