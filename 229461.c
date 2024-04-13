virSecuritySELinuxSetChildProcessLabel(virSecurityManager *mgr G_GNUC_UNUSED,
                                       virDomainDef *def,
                                       virCommand *cmd)
{
    /* TODO: verify DOI */
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (!secdef || !secdef->label)
        return 0;

    VIR_DEBUG("label=%s", secdef->label);
    if (STRNEQ(SECURITY_SELINUX_NAME, secdef->model)) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("security label driver mismatch: "
                         "'%s' model configured for domain, but "
                         "hypervisor driver is '%s'."),
                       secdef->model, SECURITY_SELINUX_NAME);
        if (security_getenforce() == 1)
            return -1;
    }

    /* save in cmd to be set after fork/before child process is exec'ed */
    virCommandSetSELinuxLabel(cmd, secdef->label);
    return 0;
}