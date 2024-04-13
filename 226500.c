lr_yum_use_local(LrHandle *handle, LrResult *result, GError **err)
{
    char *baseurl;
    LrYumRepo *repo;
    LrYumRepoMd *repomd;

    assert(!err || *err == NULL);

    g_debug("%s: Locating repo..", __func__);

    // Shortcuts
    repo   = result->yum_repo;
    repomd = result->yum_repomd;
    baseurl = handle->urls[0];

    // Skip "file://" prefix if present
    if (g_str_has_prefix(baseurl, "file://"))
        baseurl += 7;
    else if (g_str_has_prefix(baseurl, "file:"))
        baseurl += 5;

    // Check sanity
    if (strstr(baseurl, "://")) {
        g_set_error(err, LR_YUM_ERROR, LRE_NOTLOCAL,
                    "URL: %s doesn't seem to be a local repository",
                    baseurl);
        return FALSE;
    }

    if (!handle->update) {
        // Load repomd.xml and mirrorlist+metalink if locally available
        if (!lr_yum_use_local_load_base(handle, result, repo, repomd, baseurl, err))
            return FALSE;
    }

    if(handle->cachedir) {
        lr_yum_switch_to_zchunk(handle, repomd);
        repo->use_zchunk = TRUE;
    } else {
        g_debug("%s: Cache directory not set, disabling zchunk", __func__);
        repo->use_zchunk = FALSE;
    }

    // Locate rest of metadata files
    for (GSList *elem = repomd->records; elem; elem = g_slist_next(elem)) {
        _cleanup_free_ char *path = NULL;
        LrYumRepoMdRecord *record = elem->data;

        assert(record);

        if (!lr_yum_repomd_record_enabled(handle, record->type, repomd->records))
            continue; // Caller isn't interested in this record type
        if (yum_repo_path(repo, record->type))
            continue; // This path already exists in repo

        path = lr_pathconcat(baseurl, record->location_href, NULL);

        if (access(path, F_OK) == -1) {
            // A repo file is missing
            if (!handle->ignoremissing) {
                g_debug("%s: Incomplete repository - %s is missing",
                        __func__, path);
                g_set_error(err, LR_YUM_ERROR, LRE_INCOMPLETEREPO,
                            "Incomplete repository - %s is missing",
                            path);
                return FALSE;
            }

            continue;
        }

        lr_yum_repo_append(repo, record->type, path);
    }

    g_debug("%s: Repository was successfully located", __func__);
    return TRUE;
}