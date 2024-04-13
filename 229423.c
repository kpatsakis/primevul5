virSecuritySELinuxSetHostdevCapsLabel(virSecurityManager *mgr,
                                      virDomainDef *def,
                                      virDomainHostdevDef *dev,
                                      const char *vroot)
{
    int ret = -1;
    virSecurityLabelDef *secdef;
    char *path;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (secdef == NULL)
        return 0;

    switch (dev->source.caps.type) {
    case VIR_DOMAIN_HOSTDEV_CAPS_TYPE_STORAGE: {
        if (vroot) {
            path = g_strdup_printf("%s/%s", vroot,
                                   dev->source.caps.u.storage.block);
        } else {
            path = g_strdup(dev->source.caps.u.storage.block);
        }
        ret = virSecuritySELinuxSetFilecon(mgr, path, secdef->imagelabel, true);
        VIR_FREE(path);
        break;
    }

    case VIR_DOMAIN_HOSTDEV_CAPS_TYPE_MISC: {
        if (vroot) {
            path = g_strdup_printf("%s/%s", vroot,
                                   dev->source.caps.u.misc.chardev);
        } else {
            path = g_strdup(dev->source.caps.u.misc.chardev);
        }
        ret = virSecuritySELinuxSetFilecon(mgr, path, secdef->imagelabel, true);
        VIR_FREE(path);
        break;
    }

    default:
        ret = 0;
        break;
    }

    return ret;
}