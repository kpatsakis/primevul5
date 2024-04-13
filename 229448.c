virSecuritySELinuxSetImageFDLabel(virSecurityManager *mgr G_GNUC_UNUSED,
                                  virDomainDef *def,
                                  int fd)
{
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->imagelabel)
        return 0;

    return virSecuritySELinuxFSetFilecon(fd, secdef->imagelabel);
}