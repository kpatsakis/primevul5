virSecuritySELinuxGetSecurityMountOptions(virSecurityManager *mgr,
                                          virDomainDef *def)
{
    char *opts = NULL;
    virSecurityLabelDef *secdef;

    if ((secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME))) {
        if (!secdef->imagelabel)
            secdef->imagelabel = virSecuritySELinuxGenImageLabel(mgr, def);

        if (secdef->imagelabel) {
            opts = g_strdup_printf(
                                   ",context=\"%s\"",
                                   (const char*) secdef->imagelabel);
        }
    }

    if (!opts)
        opts = g_strdup("");

    VIR_DEBUG("imageLabel=%s opts=%s",
              secdef ? secdef->imagelabel : "(null)", opts);
    return opts;
}