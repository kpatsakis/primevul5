virSecuritySELinuxSetMemoryLabel(virSecurityManager *mgr,
                                 virDomainDef *def,
                                 virDomainMemoryDef *mem)
{
    virSecurityLabelDef *seclabel;

    switch (mem->model) {
    case VIR_DOMAIN_MEMORY_MODEL_NVDIMM:
    case VIR_DOMAIN_MEMORY_MODEL_VIRTIO_PMEM:
        seclabel = virDomainDefGetSecurityLabelDef(def, SECURITY_SELINUX_NAME);
        if (!seclabel || !seclabel->relabel)
            return 0;

        if (virSecuritySELinuxSetFilecon(mgr, mem->nvdimmPath,
                                         seclabel->imagelabel, true) < 0)
            return -1;
        break;

    case VIR_DOMAIN_MEMORY_MODEL_NONE:
    case VIR_DOMAIN_MEMORY_MODEL_DIMM:
    case VIR_DOMAIN_MEMORY_MODEL_LAST:
        break;
    }

    return 0;
}