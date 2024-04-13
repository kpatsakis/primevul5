virSecuritySELinuxSetSocketLabel(virSecurityManager *mgr G_GNUC_UNUSED,
                                 virDomainDef *vm)
{
    virSecurityLabelDef *secdef;
    int rc = -1;

    secdef = virDomainDefGetSecurityLabelDef(vm, SECURITY_SELINUX_NAME);
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

    VIR_DEBUG("Setting VM %s socket context %s",
              vm->name, secdef->label);
    if (setsockcreatecon_raw(secdef->label) == -1) {
        virReportSystemError(errno,
                             _("unable to set socket security context '%s'"),
                             secdef->label);
        goto done;
    }

    rc = 0;

 done:
    if (security_getenforce() != 1)
        rc = 0;

    return rc;
}