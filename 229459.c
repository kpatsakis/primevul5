virSecuritySELinuxRestoreImageLabelSingle(virSecurityManager *mgr,
                                          virDomainDef *def,
                                          virStorageSource *src,
                                          bool migrated)
{
    virSecurityLabelDef *seclabel;
    virSecurityDeviceLabelDef *disk_seclabel;
    g_autofree char *vfioGroupDev = NULL;
    const char *path = src->path;

    seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
    if (seclabel == NULL)
        return 0;

    disk_seclabel = virStorageSourceGetSecurityLabelDef(src,
                                                        SECURITY_SELINUX_NAME);
    if (!seclabel->relabel || (disk_seclabel && !disk_seclabel->relabel))
        return 0;

    /* If labelskip is true and there are no backing files, then we
     * know it is safe to skip the restore.  FIXME - backing files should
     * be tracked in domain XML, at which point labelskip should be a
     * per-file attribute instead of a disk attribute. */
    if (disk_seclabel && disk_seclabel->labelskip &&
        !virStorageSourceHasBacking(src))
        return 0;

    /* Don't restore labels on readonly/shared disks, because other VMs may
     * still be accessing these. Alternatively we could iterate over all
     * running domains and try to figure out if it is in use, but this would
     * not work for clustered filesystems, since we can't see running VMs using
     * the file on other nodes. Safest bet is thus to skip the restore step. */
    if (src->readonly || src->shared)
        return 0;


    /* If we have a shared FS and are doing migration, we must not change
     * ownership, because that kills access on the destination host which is
     * sub-optimal for the guest VM's I/O attempts :-) */
    if (migrated) {
        int rc = 1;

        if (virStorageSourceIsLocalStorage(src)) {
            if (!src->path)
                return 0;

            if ((rc = virFileIsSharedFS(src->path)) < 0)
                return -1;
        }

        if (rc == 1) {
            VIR_DEBUG("Skipping image label restore on %s because FS is shared",
                      src->path);
            return 0;
        }
    }

    /* This is not very clean. But so far we don't have NVMe
     * storage pool backend so that its chownCallback would be
     * called. And this place looks least offensive. */
    if (src->type == VIR_STORAGE_TYPE_NVME) {
        const virStorageSourceNVMeDef *nvme = src->nvme;

        if (!(vfioGroupDev = virPCIDeviceAddressGetIOMMUGroupDev(&nvme->pciAddr)))
            return -1;

        /* Ideally, we would check if there is not another PCI
         * device within domain def that is in the same IOMMU
         * group. But we're not doing that for hostdevs yet. */
        path = vfioGroupDev;
    }

    return virSecuritySELinuxRestoreFileLabel(mgr, path, true);
}