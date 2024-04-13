virSecuritySELinuxSetTPMLabels(virSecurityManager *mgr,
                               virDomainDef *def)
{
    int ret = 0;
    size_t i;
    virSecurityLabelDef *seclabel;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    for (i = 0; i < def->ntpms; i++) {
        if (def->tpms[i]->type != VIR_DOMAIN_TPM_TYPE_EMULATOR)
            continue;

        ret = virSecuritySELinuxSetFileLabels(
            mgr, def->tpms[i]->data.emulator.storagepath,
            seclabel);
        if (ret == 0 && def->tpms[i]->data.emulator.logfile)
            ret = virSecuritySELinuxSetFileLabels(
                mgr, def->tpms[i]->data.emulator.logfile,
                seclabel);
    }

    return ret;
}