virSecuritySELinuxReserveLabel(virSecurityManager *mgr,
                               virDomainDef *def,
                               pid_t pid)
{
    char *pctx;
    context_t ctx = NULL;
    const char *mcs;
    int rv;
    virSecurityLabelDef *seclabel;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!seclabel ||
        seclabel->type == VIR_DOMAIN_SECLABEL_NONE ||
        seclabel->type == VIR_DOMAIN_SECLABEL_STATIC)
        return 0;

    if (getpidcon_raw(pid, &pctx) == -1) {
        virReportSystemError(errno,
                             _("unable to get PID %d security context"), pid);
        return -1;
    }

    ctx = context_new(pctx);
    freecon(pctx);
    if (!ctx)
        goto error;

    mcs = context_range_get(ctx);
    if (!mcs)
        goto error;

    if ((rv = virSecuritySELinuxMCSAdd(mgr, mcs)) < 0)
        goto error;

    if (rv == 1) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("MCS level for existing domain label %s already reserved"),
                       (char*)pctx);
        goto error;
    }

    context_free(ctx);

    return 0;

 error:
    context_free(ctx);
    return -1;
}