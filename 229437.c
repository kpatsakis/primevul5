virSecuritySELinuxTransactionCommit(virSecurityManager *mgr G_GNUC_UNUSED,
                                    pid_t pid,
                                    bool lock)
{
    virSecuritySELinuxContextList *list;
    int rc;
    int ret = -1;

    list = virThreadLocalGet(&contextList);
    if (!list) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("No transaction is set"));
        return -1;
    }

    if (virThreadLocalSet(&contextList, NULL) < 0) {
        virReportSystemError(errno, "%s",
                             _("Unable to clear thread local variable"));
        goto cleanup;
    }

    list->lock = lock;

    if (pid != -1) {
        rc = virProcessRunInMountNamespace(pid,
                                           virSecuritySELinuxTransactionRun,
                                           list);
        if (rc < 0) {
            if (virGetLastErrorCode() == VIR_ERR_SYSTEM_ERROR)
                pid = -1;
            else
                goto cleanup;
        }
    }

    if (pid == -1) {
        if (lock)
            rc = virProcessRunInFork(virSecuritySELinuxTransactionRun, list);
        else
            rc = virSecuritySELinuxTransactionRun(pid, list);
    }

    if (rc < 0)
        goto cleanup;

    ret = 0;
 cleanup:
    virSecuritySELinuxContextListFree(list);
    return ret;
}