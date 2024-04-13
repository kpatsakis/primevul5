virSecuritySELinuxDomainSetPathLabel(virSecurityManager *mgr,
                                     virDomainDef *def,
                                     const char *path,
                                     bool allowSubtree G_GNUC_UNUSED)
{
    virSecurityLabelDef *seclabel;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!seclabel || !seclabel->relabel)
        return 0;

    return virSecuritySELinuxSetFilecon(mgr, path, seclabel->imagelabel, true);
}