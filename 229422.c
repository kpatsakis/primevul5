virSecuritySELinuxTransactionRun(pid_t pid G_GNUC_UNUSED,
                                 void *opaque)
{
    virSecuritySELinuxContextList *list = opaque;
    virSecurityManagerMetadataLockState *state;
    const char **paths = NULL;
    size_t npaths = 0;
    size_t i;
    int rv;
    int ret = -1;

    if (list->lock) {
        paths = g_new0(const char *, list->nItems);

        for (i = 0; i < list->nItems; i++) {
            virSecuritySELinuxContextItem *item = list->items[i];
            const char *p = item->path;

            if (item->remember)
                VIR_APPEND_ELEMENT_COPY_INPLACE(paths, npaths, p);
        }

        if (!(state = virSecurityManagerMetadataLock(list->manager, paths, npaths)))
            goto cleanup;

        for (i = 0; i < list->nItems; i++) {
            virSecuritySELinuxContextItem *item = list->items[i];
            size_t j;

            for (j = 0; j < state->nfds; j++) {
                if (STREQ_NULLABLE(item->path, state->paths[j]))
                    break;
            }

            /* If path wasn't locked, don't try to remember its label. */
            if (j == state->nfds)
                item->remember = false;
        }
    }

    rv = 0;
    for (i = 0; i < list->nItems; i++) {
        virSecuritySELinuxContextItem *item = list->items[i];
        const bool remember = item->remember && list->lock;

        if (!item->restore) {
            rv = virSecuritySELinuxSetFilecon(list->manager,
                                              item->path,
                                              item->tcon,
                                              remember);
        } else {
            rv = virSecuritySELinuxRestoreFileLabel(list->manager,
                                                    item->path,
                                                    remember);
        }

        if (rv < 0)
            break;
    }

    for (; rv < 0 && i > 0; i--) {
        virSecuritySELinuxContextItem *item = list->items[i - 1];
        const bool remember = item->remember && list->lock;

        if (!item->restore) {
            virSecuritySELinuxRestoreFileLabel(list->manager,
                                               item->path,
                                               remember);
        } else {
            VIR_WARN("Ignoring failed restore attempt on %s", item->path);
        }
    }

    if (list->lock)
        virSecurityManagerMetadataUnlock(list->manager, &state);

    if (rv < 0)
        goto cleanup;

    ret = 0;
 cleanup:
    VIR_FREE(paths);
    return ret;
}