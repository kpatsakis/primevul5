virSecuritySELinuxTransactionStart(virSecurityManager *mgr)
{
    virSecuritySELinuxContextList *list;

    list = virThreadLocalGet(&contextList);
    if (list) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("Another relabel transaction is already started"));
        return -1;
    }

    list = g_new0(virSecuritySELinuxContextList, 1);

    list->manager = virObjectRef(mgr);

    if (virThreadLocalSet(&contextList, list) < 0) {
        virReportSystemError(errno, "%s",
                             _("Unable to set thread local variable"));
        virSecuritySELinuxContextListFree(list);
        return -1;
    }

    return 0;
}