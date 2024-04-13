virSecuritySELinuxMoveImageMetadata(virSecurityManager *mgr,
                                    pid_t pid,
                                    virStorageSource *src,
                                    virStorageSource *dst)
{
    struct virSecuritySELinuxMoveImageMetadataData data = { .mgr = mgr, 0 };
    int rc;

    if (src && virStorageSourceIsLocalStorage(src))
        data.src = src->path;

    if (dst && virStorageSourceIsLocalStorage(dst))
        data.dst = dst->path;

    if (!data.src)
        return 0;

    if (pid == -1) {
        rc = virProcessRunInFork(virSecuritySELinuxMoveImageMetadataHelper,
                                 &data);
    } else {
        rc = virProcessRunInMountNamespace(pid,
                                           virSecuritySELinuxMoveImageMetadataHelper,
                                           &data);
    }

    return rc;
}