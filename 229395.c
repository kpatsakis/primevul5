virSecuritySELinuxSetImageLabelRelative(virSecurityManager *mgr,
                                        virDomainDef *def,
                                        virStorageSource *src,
                                        virStorageSource *parent,
                                        virSecurityDomainImageLabelFlags flags)
{
    virStorageSource *n;

    for (n = src; virStorageSourceIsBacking(n); n = n->backingStore) {
        const bool isChainTop = flags & VIR_SECURITY_DOMAIN_IMAGE_PARENT_CHAIN_TOP;

        if (virSecuritySELinuxSetImageLabelInternal(mgr, def, n, parent, isChainTop) < 0)
            return -1;

        if (!(flags & VIR_SECURITY_DOMAIN_IMAGE_LABEL_BACKING_CHAIN))
            break;

        flags &= ~VIR_SECURITY_DOMAIN_IMAGE_PARENT_CHAIN_TOP;
    }

    return 0;
}