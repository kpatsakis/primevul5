lr_yum_check_checksum_of_md_record(LrYumRepoMdRecord *rec,
                                   const char *path,
                                   GError **err)
{
    int fd;
    char *expected_checksum;
    LrChecksumType checksum_type;
    gboolean ret, matches;
    gboolean is_zchunk = FALSE;
    GError *tmp_err = NULL;

    assert(!err || *err == NULL);

    if (!rec || !path)
        return TRUE;

    #ifdef WITH_ZCHUNK
    if(rec->header_checksum) {
        expected_checksum = rec->header_checksum;
        checksum_type = lr_checksum_type(rec->header_checksum_type);
        is_zchunk = TRUE;
    } else {
    #endif /* WITH_ZCHUNK */
        expected_checksum = rec->checksum;
        checksum_type = lr_checksum_type(rec->checksum_type);
    #ifdef WITH_ZCHUNK
    }
    #endif /* WITH_ZCHUNK */

    g_debug("%s: Checking checksum of %s (expected: %s [%s])",
                       __func__, path, expected_checksum, rec->checksum_type);

    if (!expected_checksum) {
        // Empty checksum - suppose it's ok
        g_debug("%s: No checksum in repomd", __func__);
        return TRUE;
    }

    if (checksum_type == LR_CHECKSUM_UNKNOWN) {
        g_debug("%s: Unknown checksum", __func__);
        g_set_error(err, LR_YUM_ERROR, LRE_UNKNOWNCHECKSUM,
                    "Unknown checksum type for %s", path);
        return FALSE;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        g_debug("%s: Cannot open %s", __func__, path);
        g_set_error(err, LR_YUM_ERROR, LRE_IO,
                    "Cannot open %s: %s", path, g_strerror(errno));
        return FALSE;
    }

    if (is_zchunk) {
        #ifdef WITH_ZCHUNK
        ret = FALSE;
        matches = FALSE;
        zckCtx *zck = lr_zck_init_read_base(expected_checksum, checksum_type,
                                            rec->size_header, fd, &tmp_err);
        if (!tmp_err) {
            if(zck_validate_checksums(zck) < 1) {
                g_set_error(&tmp_err, LR_YUM_ERROR, LRE_ZCK,
                            "Unable to validate zchunk checksums");
            } else {
                ret = TRUE;
                matches = TRUE;
            }
        }
        if (zck)
            zck_free(&zck);
        #endif /* WITH_ZCHUNK */
    } else {
        ret = lr_checksum_fd_cmp(checksum_type,
                                 fd,
                                 expected_checksum,
                                 1,
                                 &matches,
                                 &tmp_err);
    }

    close(fd);

    assert(ret || tmp_err);

    if (!ret) {
        // Checksum calculation error
        g_debug("%s: Checksum check %s - Error: %s",
                __func__, path, tmp_err->message);
        g_propagate_prefixed_error(err, tmp_err,
                                   "Checksum error %s: ", path);
        return FALSE;
    } else if (!matches) {
        g_debug("%s: Checksum check %s - Mismatch", __func__, path);
        g_set_error(err, LR_YUM_ERROR, LRE_BADCHECKSUM,
                    "Checksum mismatch %s", path);
        return FALSE;
    }

    g_debug("%s: Checksum check - Passed", __func__);

    return TRUE;
}