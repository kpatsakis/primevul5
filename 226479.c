lr_yum_use_local_load_base(LrHandle *handle,
                           LrResult *result,
                           LrYumRepo *repo,
                           LrYumRepoMd *repomd,
                           const gchar *baseurl,
                           GError **err)
{
    gboolean ret;
    GError *tmp_err = NULL;
    _cleanup_free_ gchar *path = NULL;
    _cleanup_free_ gchar *sig = NULL;
    _cleanup_fd_close_ int fd = -1;

    if (handle->mirrorlist_fd != -1) {
        // Locate mirrorlist if available.
        gchar *mrl_fn = lr_pathconcat(baseurl, "mirrorlist", NULL);
        if (g_file_test(mrl_fn, G_FILE_TEST_IS_REGULAR)) {
            g_debug("%s: Found local mirrorlist: %s", __func__, mrl_fn);
            repo->mirrorlist = mrl_fn;
        } else {
            repo->mirrorlist = NULL;
            lr_free(mrl_fn);
        }
    }

    if (handle->metalink_fd != -1) {
        // Locate metalink.xml if available.
        gchar *mtl_fn = lr_pathconcat(baseurl, "metalink.xml", NULL);
        if (g_file_test(mtl_fn, G_FILE_TEST_IS_REGULAR)) {
            g_debug("%s: Found local metalink: %s", __func__, mtl_fn);
            repo->metalink = mtl_fn;
        } else {
            repo->metalink = NULL;
            lr_free(mtl_fn);
        }
    }

    // Open repomd.xml
    path = lr_pathconcat(baseurl, "repodata/repomd.xml", NULL);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        g_debug("%s: open(%s): %s", __func__, path, g_strerror(errno));
        g_set_error(err, LR_YUM_ERROR, LRE_IO,
                    "Cannot open %s: %s", path, g_strerror(errno));
        return FALSE;
    }

    // Parse repomd.xml
    g_debug("%s: Parsing repomd.xml", __func__);
    ret = lr_yum_repomd_parse_file(repomd, fd, lr_xml_parser_warning_logger,
                                   "Repomd xml parser", &tmp_err);
    if (!ret) {
        g_debug("%s: Parsing unsuccessful: %s", __func__, tmp_err->message);
        g_propagate_prefixed_error(err, tmp_err,
                                   "repomd.xml parser error: ");
        return FALSE;
    }

    // Fill result object
    result->destdir = g_strdup(baseurl);
    repo->destdir = g_strdup(baseurl);
    repo->repomd = g_strdup(path);

    // Check if signature file exists
    sig = lr_pathconcat(baseurl, "repodata/repomd.xml.asc", NULL);
    if (access(sig, F_OK) == 0)
        repo->signature = g_strdup(sig);

    // Signature checking
    if (handle->checks & LR_CHECK_GPG) {

        if (!repo->signature) {
            // Signature doesn't exist
            g_set_error(err, LR_YUM_ERROR, LRE_BADGPG,
                        "GPG verification is enabled, but GPG signature "
                        "repomd.xml.asc is not available. This may be an "
                        "error or the repository does not support GPG verification.");
            return FALSE;
        }

        ret = lr_gpg_check_signature(repo->signature,
                                     repo->repomd,
                                     handle->gnupghomedir,
                                     &tmp_err);
        if (!ret) {
            g_debug("%s: repomd.xml GPG signature verification failed: %s",
                    __func__, tmp_err->message);
            g_propagate_prefixed_error(err, tmp_err,
                        "repomd.xml GPG signature verification failed: ");
            return FALSE;
        }
    }

    // Done - repomd is loaded and checked
    g_debug("%s: Repomd revision: %s", __func__, repomd->revision);

    return TRUE;
}