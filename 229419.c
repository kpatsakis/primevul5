virSecuritySELinuxSetSavedStateLabel(virSecurityManager *mgr,
                                     virDomainDef *def,
                                     const char *savefile)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);

    if (!savefile || !secdef || !secdef->relabel || data->skipAllLabel)
        return 0;

    return virSecuritySELinuxSetFilecon(mgr, savefile, data->content_context, false);
}