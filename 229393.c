virSecuritySELinuxGenImageLabel(virSecurityManager *mgr,
                                virDomainDef *def)
{
    virSecurityLabelDef *secdef;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);
    const char *range;
    context_t ctx = NULL;
    char *label = NULL;
    char *mcs = NULL;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (secdef == NULL)
        goto cleanup;

    if (secdef->label) {
        ctx = context_new(secdef->label);
        if (!ctx) {
            virReportSystemError(errno, _("unable to create selinux context for: %s"),
                                 secdef->label);
            goto cleanup;
        }
        range = context_range_get(ctx);
        if (range) {
            mcs = g_strdup(range);
            if (!(label = virSecuritySELinuxGenNewContext(data->file_context,
                                                          mcs, true)))
                goto cleanup;
        }
    }

 cleanup:
    context_free(ctx);
    VIR_FREE(mcs);
    return label;
}