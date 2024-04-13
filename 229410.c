virSecuritySELinuxContextItemFree(virSecuritySELinuxContextItem *item)
{
    if (!item)
        return;

    g_free(item->path);
    g_free(item->tcon);
    g_free(item);
}