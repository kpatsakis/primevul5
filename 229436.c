virSecuritySELinuxRestoreHostdevCapsLabel(virSecurityManager *mgr,
                                          virDomainHostdevDef *dev,
                                          const char *vroot)
{
    int ret = -1;
    char *path;

    switch (dev->source.caps.type) {
    case VIR_DOMAIN_HOSTDEV_CAPS_TYPE_STORAGE: {
        if (vroot) {
            path = g_strdup_printf("%s/%s", vroot,
                                   dev->source.caps.u.storage.block);
        } else {
            path = g_strdup(dev->source.caps.u.storage.block);
        }
        ret = virSecuritySELinuxRestoreFileLabel(mgr, path, true);
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
        ret = virSecuritySELinuxRestoreFileLabel(mgr, path, true);
        VIR_FREE(path);
        break;
    }

    default:
        ret = 0;
        break;
    }

    return ret;
}