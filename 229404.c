virSecuritySELinuxSetImageLabel(virSecurityManager *mgr,
                                virDomainDef *def,
                                virStorageSource *src,
                                virSecurityDomainImageLabelFlags flags)
{
    return virSecuritySELinuxSetImageLabelRelative(mgr, def, src, src, flags);
}