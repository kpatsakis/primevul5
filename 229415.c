virSecuritySELinuxSetAllLabel(virSecurityManager *mgr,
                              virDomainDef *def,
                              const char *incomingPath G_GNUC_UNUSED,
                              bool chardevStdioLogd,
                              bool migrated G_GNUC_UNUSED)
{
    size_t i;
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);
    virSecurityLabelDef *secdef;

    struct _virSecuritySELinuxChardevCallbackData chardevData = {
        .mgr = mgr,
        .chardevStdioLogd = chardevStdioLogd
    };

    secdef = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);

    if (!secdef || !secdef->relabel || data->skipAllLabel)
        return 0;

    for (i = 0; i < def->ndisks; i++) {
        /* XXX fixme - we need to recursively label the entire tree :-( */
        if (virDomainDiskGetType(def->disks[i]) == VIR_STORAGE_TYPE_DIR) {
            VIR_WARN("Unable to relabel directory tree %s for disk %s",
                     virDomainDiskGetSource(def->disks[i]),
                     def->disks[i]->dst);
            continue;
        }
        if (virSecuritySELinuxSetImageLabel(mgr, def, def->disks[i]->src,
                                            VIR_SECURITY_DOMAIN_IMAGE_LABEL_BACKING_CHAIN |
                                            VIR_SECURITY_DOMAIN_IMAGE_PARENT_CHAIN_TOP) < 0)
            return -1;
    }
    /* XXX fixme process  def->fss if relabel == true */

    for (i = 0; i < def->nhostdevs; i++) {
        if (virSecuritySELinuxSetHostdevLabel(mgr,
                                              def,
                                              def->hostdevs[i],
                                              NULL) < 0)
            return -1;
    }

    for (i = 0; i < def->ninputs; i++) {
        if (virSecuritySELinuxSetInputLabel(mgr, def, def->inputs[i]) < 0)
            return -1;
    }

    for (i = 0; i < def->nmems; i++) {
        if (virSecuritySELinuxSetMemoryLabel(mgr, def, def->mems[i]) < 0)
            return -1;
    }

    for (i = 0; i < def->ntpms; i++) {
        if (virSecuritySELinuxSetTPMFileLabel(mgr, def, def->tpms[i]) < 0)
            return -1;
    }

    if (virDomainChrDefForeach(def,
                               true,
                               virSecuritySELinuxSetSecurityChardevCallback,
                               &chardevData) < 0)
        return -1;

    if (virDomainSmartcardDefForeach(def,
                                     true,
                                     virSecuritySELinuxSetSecuritySmartcardCallback,
                                     mgr) < 0)
        return -1;

    for (i = 0; i < def->nsysinfo; i++) {
        if (virSecuritySELinuxSetSysinfoLabel(mgr,
                                              def->sysinfo[i],
                                              data) < 0)
            return -1;
    }

    /* This is different than kernel or initrd. The nvram store
     * is really a disk, qemu can read and write to it. */
    if (def->os.loader && def->os.loader->nvram &&
        secdef && secdef->imagelabel &&
        virSecuritySELinuxSetFilecon(mgr, def->os.loader->nvram,
                                     secdef->imagelabel, true) < 0)
        return -1;

    if (def->os.kernel &&
        virSecuritySELinuxSetFilecon(mgr, def->os.kernel,
                                     data->content_context, true) < 0)
        return -1;

    if (def->os.initrd &&
        virSecuritySELinuxSetFilecon(mgr, def->os.initrd,
                                     data->content_context, true) < 0)
        return -1;

    if (def->os.dtb &&
        virSecuritySELinuxSetFilecon(mgr, def->os.dtb,
                                     data->content_context, true) < 0)
        return -1;

    if (def->os.slic_table &&
        virSecuritySELinuxSetFilecon(mgr, def->os.slic_table,
                                     data->content_context, true) < 0)
        return -1;

    return 0;
}