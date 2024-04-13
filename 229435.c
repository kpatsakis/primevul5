virSecuritySELinuxRestoreFileLabel(virSecurityManager *mgr,
                                   const char *path,
                                   bool recall)
{
    bool privileged = virSecurityManagerGetPrivileged(mgr);
    struct stat buf;
    char *fcon = NULL;
    char *newpath = NULL;
    int rc;
    int ret = -1;

    /* Some paths are auto-generated, so let's be safe here and do
     * nothing if nothing is needed.
     */
    if (!path)
        return 0;

    VIR_INFO("Restoring SELinux context on '%s'", path);

    if (virFileResolveLink(path, &newpath) < 0) {
        VIR_WARN("cannot resolve symlink %s: %s", path,
                 g_strerror(errno));
        goto cleanup;
    }

    if ((rc = virSecuritySELinuxTransactionAppend(path, NULL,
                                                  recall, true)) < 0) {
        goto cleanup;
    } else if (rc > 0) {
        ret = 0;
        goto cleanup;
    }

    if (recall) {
        rc = virSecuritySELinuxRecallLabel(newpath, &fcon);
        if (rc == -2) {
            /* Not supported. Lookup the default label below. */
        } else if (rc < 0) {
            goto cleanup;
        } else if (rc > 0) {
            ret = 0;
            goto cleanup;
        }
    }

    if (!recall || rc == -2) {
        if (stat(newpath, &buf) != 0) {
            VIR_WARN("cannot stat %s: %s", newpath,
                     g_strerror(errno));
            goto cleanup;
        }

        if (getContext(mgr, newpath, buf.st_mode, &fcon) < 0) {
            /* Any user created path likely does not have a default label,
             * which makes this an expected non error
             */
            VIR_WARN("cannot lookup default selinux label for %s", newpath);
            ret = 0;
            goto cleanup;
        }
    }

    if (virSecuritySELinuxSetFileconImpl(newpath, fcon, privileged) < 0)
        goto cleanup;

    ret = 0;
 cleanup:
    freecon(fcon);
    VIR_FREE(newpath);
    return ret;
}