virSecuritySELinuxRestoreImageLabelInt(virSecurityManager *mgr,
                                       virDomainDef *def,
                                       virStorageSource *src,
                                       bool migrated)
{
    if (virSecuritySELinuxRestoreImageLabelSingle(mgr, def, src, migrated) < 0)
        return -1;

    return 0;
}