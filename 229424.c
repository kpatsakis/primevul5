virSecuritySELinuxGetProcessLabel(virSecurityManager *mgr G_GNUC_UNUSED,
                                  virDomainDef *def G_GNUC_UNUSED,
                                  pid_t pid,
                                  virSecurityLabelPtr sec)
{
    char *ctx;

    if (getpidcon_raw(pid, &ctx) == -1) {
        virReportSystemError(errno,
                             _("unable to get PID %d security context"),
                             pid);
        return -1;
    }

    if (virStrcpy(sec->label, ctx, VIR_SECURITY_LABEL_BUFLEN) < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("security label exceeds "
                         "maximum length: %d"),
                       VIR_SECURITY_LABEL_BUFLEN - 1);
        freecon(ctx);
        return -1;
    }

    freecon(ctx);

    VIR_DEBUG("label=%s", sec->label);
    sec->enforcing = security_getenforce();
    if (sec->enforcing == -1) {
        virReportSystemError(errno, "%s",
                             _("error calling security_getenforce()"));
        return -1;
    }

    return 0;
}