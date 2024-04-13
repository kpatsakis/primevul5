virSecuritySELinuxRestoreChardevLabel(virSecurityManager *mgr,
                                      virDomainDef *def,
                                      virDomainChrSourceDef *dev_source,
                                      bool chardevStdioLogd)

{
    virSecurityLabelDef *seclabel;
    virSecurityDeviceLabelDef *chr_seclabel = NULL;
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

    switch (dev_source->type) {
    case VIR_DOMAIN_CHR_TYPE_DEV:
    case VIR_DOMAIN_CHR_TYPE_FILE:
        if (virSecuritySELinuxRestoreFileLabel(mgr,
                                               dev_source->data.file.path,
                                               true) < 0)
            goto done;
        ret = 0;
        break;

    case VIR_DOMAIN_CHR_TYPE_UNIX:
        if (!dev_source->data.nix.listen) {
            if (virSecuritySELinuxRestoreFileLabel(mgr,
                                                   dev_source->data.file.path,
                                                   true) < 0)
                goto done;
        }
        ret = 0;
        break;

    case VIR_DOMAIN_CHR_TYPE_PIPE:
        out = g_strdup_printf("%s.out", dev_source->data.file.path);
        in = g_strdup_printf("%s.in", dev_source->data.file.path);
        if (virFileExists(in) && virFileExists(out)) {
            if ((virSecuritySELinuxRestoreFileLabel(mgr, out, true) < 0) ||
                (virSecuritySELinuxRestoreFileLabel(mgr, in, true) < 0)) {
                goto done;
            }
        } else if (virSecuritySELinuxRestoreFileLabel(mgr,
                                                      dev_source->data.file.path,
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