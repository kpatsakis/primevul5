virSecuritySELinuxRestoreTPMLabels(virSecurityManager *mgr,
                                   virDomainDef *def)
{
    int ret = 0;
    size_t i;

    for (i = 0; i < def->ntpms; i++) {
        if (def->tpms[i]->type != VIR_DOMAIN_TPM_TYPE_EMULATOR)
            continue;

        ret = virSecuritySELinuxRestoreFileLabels(
            mgr, def->tpms[i]->data.emulator.storagepath);
        if (ret == 0 && def->tpms[i]->data.emulator.logfile)
            ret = virSecuritySELinuxRestoreFileLabels(
                mgr, def->tpms[i]->data.emulator.logfile);
    }

    return ret;
}