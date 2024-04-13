virSecuritySELinuxTransactionAbort(virSecurityManager *mgr G_GNUC_UNUSED)
{
    virSecuritySELinuxContextList *list;

    list = virThreadLocalGet(&contextList);
    if (!list)
        return;

    if (virThreadLocalSet(&contextList, NULL) < 0)
        VIR_DEBUG("Unable to clear thread local variable");
    virSecuritySELinuxContextListFree(list);
}