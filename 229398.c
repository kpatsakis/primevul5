virSecuritySELinuxContextListFree(void *opaque)
{
    virSecuritySELinuxContextList *list = opaque;
    size_t i;

    if (!list)
        return;

    for (i = 0; i < list->nItems; i++)
        virSecuritySELinuxContextItemFree(list->items[i]);

    g_free(list->items);
    virObjectUnref(list->manager);
    g_free(list);
}