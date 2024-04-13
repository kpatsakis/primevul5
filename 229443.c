virSecuritySELinuxSetFilecon(virSecurityManager *mgr,
                             const char *path,
                             const char *tcon,
                             bool remember)
{
    bool privileged = virSecurityManagerGetPrivileged(mgr);
    char *econ = NULL;
    int refcount;
    int rc;
    bool rollback = false;
    int ret = -1;

    if ((rc = virSecuritySELinuxTransactionAppend(path, tcon,
                                                  remember, false)) < 0)
        return -1;
    else if (rc > 0)
        return 0;

    if (remember) {
        if (getfilecon_raw(path, &econ) < 0 &&
            errno != ENOTSUP && errno != ENODATA) {
            virReportSystemError(errno,
                                 _("unable to get SELinux context of %s"),
                                 path);
            goto cleanup;
        }

        if (econ) {
            refcount = virSecuritySELinuxRememberLabel(path, econ);
            if (refcount > 0)
                rollback = true;
            if (refcount == -2) {
                /* Not supported. Don't error though. */
            } else if (refcount < 0) {
                goto cleanup;
            } else if (refcount > 1) {
                /* Refcount is greater than 1 which means that there
                 * is @refcount domains using the @path. Do not
                 * change the label (as it would almost certainly
                 * cause the other domains to lose access to the
                 * @path). However, the refcounter was
                 * incremented in XATTRs so decrease it. */
                if (STRNEQ(econ, tcon)) {
                    virReportError(VIR_ERR_OPERATION_INVALID,
                                   _("Setting different SELinux label on %s "
                                     "which is already in use"), path);
                    goto cleanup;
                }
            }
        }
    }

    rc = virSecuritySELinuxSetFileconImpl(path, tcon, privileged);
    if (rc < 0)
        goto cleanup;

    /* Do not try restoring the label if it was not changed
     * (setting it failed in a non-critical fashion) */
    if (rc == 0)
        rollback = false;

    ret = 0;
 cleanup:
    if (rollback) {
        virErrorPtr origerr;

        virErrorPreserveLast(&origerr);
        /* Try to restore the label. This is done so that XATTRs
         * are left in the same state as when the control entered
         * this function. However, if our attempt fails, there's
         * not much we can do. XATTRs refcounting is fubar'ed and
         * the only option we have is warn users. */
        if (virSecuritySELinuxRestoreFileLabel(mgr, path, remember) < 0)
            VIR_WARN("Unable to restore label on '%s'. "
                     "XATTRs might have been left in inconsistent state.",
                     path);

        virErrorRestore(&origerr);

    }
    freecon(econ);
    return ret;
}