virSecuritySELinuxRestoreImageLabel(virSecurityManager *mgr,
                                    virDomainDef *def,
                                    virStorageSource *src,
                                    virSecurityDomainImageLabelFlags flags G_GNUC_UNUSED)
{
    return virSecuritySELinuxRestoreImageLabelInt(mgr, def, src, false);
}