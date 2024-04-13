virSecuritySELinuxVerify(virSecurityManager *mgr G_GNUC_UNUSED,
                         virDomainDef *def)
{
    virSecurityLabelDef *secdef;

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (secdef == NULL)
        return 0;

    if (STRNEQ(SECURITY_SELINUX_NAME, secdef->model)) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("security label driver mismatch: "
                         "'%s' model configured for domain, but "
                         "hypervisor driver is '%s'."),
                       secdef->model, SECURITY_SELINUX_NAME);
        return -1;
    }

    if (secdef->type == VIR_DOMAIN_SECLABEL_STATIC) {
        if (security_check_context(secdef->label) != 0) {
            virReportError(VIR_ERR_XML_ERROR,
                           _("Invalid security label %s"), secdef->label);
            return -1;
        }
    }
    return 0;
}