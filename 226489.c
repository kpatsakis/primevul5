lr_yum_perform(LrHandle *handle, LrResult *result, GError **err)
{
    int ret = TRUE;
    LrYumRepo *repo;
    LrYumRepoMd *repomd;

    assert(handle);
    assert(!err || *err == NULL);

    if (!result) {
        g_set_error(err, LR_YUM_ERROR, LRE_BADFUNCARG,
                    "Missing result parameter");
        return FALSE;
    }

    if (!handle->urls && !handle->mirrorlisturl && !handle->metalinkurl) {
        g_set_error(err, LR_YUM_ERROR, LRE_NOURL,
                "No LRO_URLS, LRO_MIRRORLISTURL nor LRO_METALINKURL specified");
        return FALSE;
    }

    if (handle->local && (!handle->urls || !handle->urls[0])) {
        g_set_error(err, LR_YUM_ERROR, LRE_NOURL,
                    "Localrepo specified, but no LRO_URLS set");
        return FALSE;
    }

    if (handle->update) {
        // Download/Locate only specified files
        if (!result->yum_repo || !result->yum_repomd) {
            g_set_error(err, LR_YUM_ERROR, LRE_INCOMPLETERESULT,
                    "Incomplete result object - "
                    "Cannot update on this result object");
            return FALSE;
        }
    } else {
        // Download/Locate from scratch
        if (result->yum_repo || result->yum_repomd) {
            g_set_error(err, LR_YUM_ERROR, LRE_ALREADYUSEDRESULT,
                        "This result object is not clear - "
                        "Already used result object");
            return FALSE;
        }
        result->yum_repo = lr_yum_repo_init();
        result->yum_repomd = lr_yum_repomd_init();
    }

    repo   = result->yum_repo;
    repomd = result->yum_repomd;

    if (handle->local) {
        // Do not duplicate repository, just use the existing local one

        ret = lr_yum_use_local(handle, result, err);
        if (!ret)
            return FALSE;

        if (handle->checks & LR_CHECK_CHECKSUM)
            ret = lr_yum_check_repo_checksums(repo, repomd, err);
    } else {
        // Download remote/Duplicate local repository
        // Note: All checksums are checked while downloading

        ret = lr_yum_download_remote(handle, result, err);
    }

    return ret;
}