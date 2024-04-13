virSecuritySELinuxContextListAppend(virSecuritySELinuxContextList *list,
                                    const char *path,
                                    const char *tcon,
                                    bool remember,
                                    bool restore)
{
    int ret = -1;
    virSecuritySELinuxContextItem *item = NULL;

    item = g_new0(virSecuritySELinuxContextItem, 1);

    item->path = g_strdup(path);
    item->tcon = g_strdup(tcon);

    item->remember = remember;
    item->restore = restore;

    if (VIR_APPEND_ELEMENT(list->items, list->nItems, item) < 0)
        goto cleanup;

    ret = 0;
 cleanup:
    virSecuritySELinuxContextItemFree(item);
    return ret;
}