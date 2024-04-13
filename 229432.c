virSecuritySELinuxDomainSetPathLabelRO(virSecurityManager *mgr,
                                       virDomainDef *def,
                                       const char *path)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);

    if (!path || !secdef || !secdef->relabel || data->skipAllLabel)
        return 0;

    return virSecuritySELinuxSetFilecon(mgr, path, data->content_context, false);
}