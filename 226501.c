lr_yum_check_repo_checksums(LrYumRepo *repo,
                            LrYumRepoMd *repomd,
                            GError **err)
{
    assert(!err || *err == NULL);

    for (GSList *elem = repomd->records; elem; elem = g_slist_next(elem)) {
        gboolean ret;
        LrYumRepoMdRecord *record = elem->data;

        assert(record);

        const char *path = yum_repo_path(repo, record->type);

        ret = lr_yum_check_checksum_of_md_record(record, path, err);
        if (!ret)
            return FALSE;
    }

    return TRUE;
}