virSecuritySELinuxSetDaemonSocketLabel(virSecurityManager *mgr G_GNUC_UNUSED,
                                       virDomainDef *def)
{
    /* TODO: verify DOI */
    virSecurityLabelDef *secdef;
    char *scon = NULL;
    char *str = NULL;
    int rc = -1;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->label)
        return 0;

    if (STRNEQ(SECURITY_SELINUX_NAME, secdef->model)) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("security label driver mismatch: "
                         "'%s' model configured for domain, but "
                         "hypervisor driver is '%s'."),
                       secdef->model, SECURITY_SELINUX_NAME);
        goto done;
    }

    if (getcon_raw(&scon) == -1) {
        virReportSystemError(errno,
                             _("unable to get current process context '%s'"),
                             secdef->label);
        goto done;
    }

    if (!(str = virSecuritySELinuxContextAddRange(secdef->label, scon)))
        goto done;

    VIR_DEBUG("Setting VM %s socket context %s", def->name, str);
    if (setsockcreatecon_raw(str) == -1) {
        virReportSystemError(errno,
                             _("unable to set socket security context '%s'"), str);
        goto done;
    }

    rc = 0;
 done:

    if (security_getenforce() != 1)
        rc = 0;
    freecon(scon);
    VIR_FREE(str);
    return rc;
}