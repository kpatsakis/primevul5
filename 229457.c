virSecuritySELinuxRestoreHostdevLabel(virSecurityManager *mgr,
                                      virDomainDef *def,
                                      virDomainHostdevDef *dev,
                                      const char *vroot)

{
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->relabel)
        return 0;

    switch (dev->mode) {
    case VIR_DOMAIN_HOSTDEV_MODE_SUBSYS:
        return virSecuritySELinuxRestoreHostdevSubsysLabel(mgr, dev, vroot);

    case VIR_DOMAIN_HOSTDEV_MODE_CAPABILITIES:
        return virSecuritySELinuxRestoreHostdevCapsLabel(mgr, dev, vroot);

    default:
        return 0;
    }
}