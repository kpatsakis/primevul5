virSecuritySELinuxTransactionAppend(const char *path,
                                    const char *tcon,
                                    bool remember,
                                    bool restore)
{
    virSecuritySELinuxContextList *list;

    list = virThreadLocalGet(&contextList);
    if (!list)
        return 0;

    if (virSecuritySELinuxContextListAppend(list, path, tcon,
                                            remember, restore) < 0)
        return -1;

    return 1;
}