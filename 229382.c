virSecuritySELinuxRestoreAllLabel(virSecurityManager *mgr,
                                  virDomainDef *def,
                                  bool migrated,
                                  bool chardevStdioLogd)
{
    virSecurityLabelDef *secdef;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);
    size_t i;
    int rc = 0;

    struct _virSecuritySELinuxChardevCallbackData chardevData = {
        .mgr = mgr,
        .chardevStdioLogd = chardevStdioLogd
    };

    VIR_DEBUG("Restoring security label on %s migrated=%d", def->name, migrated);

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);

    if (!secdef || !secdef->relabel || data->skipAllLabel)
        return 0;

    for (i = 0; i < def->ndisks; i++) {
        virDomainDiskDef *disk = def->disks[i];

        if (virSecuritySELinuxRestoreImageLabelInt(mgr, def, disk->src,
                                                   migrated) < 0)
            rc = -1;
    }

    for (i = 0; i < def->nhostdevs; i++) {
        if (virSecuritySELinuxRestoreHostdevLabel(mgr,
                                                  def,
                                                  def->hostdevs[i],
                                                  NULL) < 0)
            rc = -1;
    }

    for (i = 0; i < def->ninputs; i++) {
        if (virSecuritySELinuxRestoreInputLabel(mgr, def, def->inputs[i]) < 0)
            rc = -1;
    }

    for (i = 0; i < def->nmems; i++) {
        if (virSecuritySELinuxRestoreMemoryLabel(mgr, def, def->mems[i]) < 0)
            return -1;
    }

    for (i = 0; i < def->ntpms; i++) {
        if (virSecuritySELinuxRestoreTPMFileLabelInt(mgr, def, def->tpms[i]) < 0)
            rc = -1;
    }

    if (virDomainChrDefForeach(def,
                               false,
                               virSecuritySELinuxRestoreSecurityChardevCallback,
                               &chardevData) < 0)
        rc = -1;

    if (virDomainSmartcardDefForeach(def,
                                     false,
                                     virSecuritySELinuxRestoreSecuritySmartcardCallback,
                                     mgr) < 0)
        rc = -1;

    for (i = 0; i < def->nsysinfo; i++) {
        if (virSecuritySELinuxRestoreSysinfoLabel(mgr, def->sysinfo[i]) < 0)
            rc = -1;
    }

    if (def->os.loader && def->os.loader->nvram &&
        virSecuritySELinuxRestoreFileLabel(mgr, def->os.loader->nvram, true) < 0)
        rc = -1;

    if (def->os.kernel &&
        virSecuritySELinuxRestoreFileLabel(mgr, def->os.kernel, true) < 0)
        rc = -1;

    if (def->os.initrd &&
        virSecuritySELinuxRestoreFileLabel(mgr, def->os.initrd, true) < 0)
        rc = -1;

    if (def->os.dtb &&
        virSecuritySELinuxRestoreFileLabel(mgr, def->os.dtb, true) < 0)
        rc = -1;

    if (def->os.slic_table &&
        virSecuritySELinuxRestoreFileLabel(mgr, def->os.slic_table, true) < 0)
        rc = -1;

    return rc;
}