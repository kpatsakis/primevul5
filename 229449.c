virSecuritySELinuxDomainRestorePathLabel(virSecurityManager *mgr,
                                         virDomainDef *def,
                                         const char *path)
{
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->relabel)
        return 0;

    return virSecuritySELinuxRestoreFileLabel(mgr, path, true);
}