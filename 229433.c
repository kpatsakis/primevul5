virSecuritySELinuxSetChardevLabel(virSecurityManager *mgr,
                                  virDomainDef *def,
                                  virDomainChrSourceDef *dev_source,
                                  bool chardevStdioLogd)

{
    virSecurityLabelDef *seclabel;
    virSecurityDeviceLabelDef *chr_seclabel = NULL;
    char *imagelabel = NULL;
    char *in = NULL, *out = NULL;
    int ret = -1;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!seclabel || !seclabel->relabel)
        return 0;

    chr_seclabel = virDomainChrSourceDefGetSecurityLabelDef(dev_source,
                                                            SECURITY_SELINUX_NAME);

    if (chr_seclabel && !chr_seclabel->relabel)
        return 0;

    if (!chr_seclabel &&
        dev_source->type == VIR_DOMAIN_CHR_TYPE_FILE &&
        chardevStdioLogd)
        return 0;

    if (chr_seclabel)
        imagelabel = chr_seclabel->label;
    if (!imagelabel)
        imagelabel = seclabel->imagelabel;

    switch (dev_source->type) {
    case VIR_DOMAIN_CHR_TYPE_DEV:
    case VIR_DOMAIN_CHR_TYPE_FILE:
        ret = virSecuritySELinuxSetFilecon(mgr,
                                           dev_source->data.file.path,
                                           imagelabel,
                                           true);
        break;

    case VIR_DOMAIN_CHR_TYPE_UNIX:
        if (!dev_source->data.nix.listen) {
            if (virSecuritySELinuxSetFilecon(mgr,
                                             dev_source->data.nix.path,
                                             imagelabel,
                                             true) < 0)
                goto done;
        }
        ret = 0;
        break;

    case VIR_DOMAIN_CHR_TYPE_PIPE:
        in = g_strdup_printf("%s.in", dev_source->data.file.path);
        out = g_strdup_printf("%s.out", dev_source->data.file.path);
        if (virFileExists(in) && virFileExists(out)) {
            if ((virSecuritySELinuxSetFilecon(mgr, in, imagelabel, true) < 0) ||
                (virSecuritySELinuxSetFilecon(mgr, out, imagelabel, true) < 0)) {
                goto done;
            }
        } else if (virSecuritySELinuxSetFilecon(mgr,
                                                dev_source->data.file.path,
                                                imagelabel,
                                                true) < 0) {
            goto done;
        }
        ret = 0;
        break;

    default:
        ret = 0;
        break;
    }

 done:
    VIR_FREE(in);
    VIR_FREE(out);
    return ret;
}