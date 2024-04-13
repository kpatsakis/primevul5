virSecuritySELinuxRestoreSavedStateLabel(virSecurityManager *mgr,
                                         virDomainDef *def,
                                         const char *savefile)
{
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->relabel)
        return 0;

    return virSecuritySELinuxRestoreFileLabel(mgr, savefile, true);
}