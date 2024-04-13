virSecuritySELinuxRestoreMemoryLabel(virSecurityManager *mgr,
                                     virDomainDef *def,
                                     virDomainMemoryDef *mem)
{
    int ret = -1;
    virSecurityLabelDef *seclabel;

    switch (mem->model) {
    case VIR_DOMAIN_MEMORY_MODEL_NVDIMM:
    case VIR_DOMAIN_MEMORY_MODEL_VIRTIO_PMEM:
        seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
        if (!seclabel || !seclabel->relabel)
            return 0;

        ret = virSecuritySELinuxRestoreFileLabel(mgr, mem->nvdimmPath, true);
        break;

    case VIR_DOMAIN_MEMORY_MODEL_DIMM:
    case VIR_DOMAIN_MEMORY_MODEL_NONE:
    case VIR_DOMAIN_MEMORY_MODEL_LAST:
        ret = 0;
        break;
    }

    return ret;
}