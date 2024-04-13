address_space_translate_for_iotlb(CPUState *cpu, int asidx, hwaddr orig_addr,
                                  hwaddr *xlat, hwaddr *plen,
                                  MemTxAttrs attrs, int *prot)
{
    MemoryRegionSection *section;
    IOMMUMemoryRegion *iommu_mr;
    IOMMUMemoryRegionClass *imrc;
    IOMMUTLBEntry iotlb;
    int iommu_idx;
    hwaddr addr = orig_addr;
    AddressSpaceDispatch *d =
        qatomic_rcu_read(&cpu->cpu_ases[asidx].memory_dispatch);

    for (;;) {
        section = address_space_translate_internal(d, addr, &addr, plen, false);

        iommu_mr = memory_region_get_iommu(section->mr);
        if (!iommu_mr) {
            break;
        }

        imrc = memory_region_get_iommu_class_nocheck(iommu_mr);

        iommu_idx = imrc->attrs_to_index(iommu_mr, attrs);
        tcg_register_iommu_notifier(cpu, iommu_mr, iommu_idx);
        /* We need all the permissions, so pass IOMMU_NONE so the IOMMU
         * doesn't short-cut its translation table walk.
         */
        iotlb = imrc->translate(iommu_mr, addr, IOMMU_NONE, iommu_idx);
        addr = ((iotlb.translated_addr & ~iotlb.addr_mask)
                | (addr & iotlb.addr_mask));
        /* Update the caller's prot bits to remove permissions the IOMMU
         * is giving us a failure response for. If we get down to no
         * permissions left at all we can give up now.
         */
        if (!(iotlb.perm & IOMMU_RO)) {
            *prot &= ~(PAGE_READ | PAGE_EXEC);
        }
        if (!(iotlb.perm & IOMMU_WO)) {
            *prot &= ~PAGE_WRITE;
        }

        if (!*prot) {
            goto translate_fail;
        }

        d = flatview_to_dispatch(address_space_to_flatview(iotlb.target_as));
    }

    assert(!memory_region_is_iommu(section->mr));
    *xlat = addr;
    return section;

translate_fail:
    /*
     * We should be given a page-aligned address -- certainly
     * tlb_set_page_with_attrs() does so.  The page offset of xlat
     * is used to index sections[], and PHYS_SECTION_UNASSIGNED = 0.
     * The page portion of xlat will be logged by memory_region_access_valid()
     * when this memory access is rejected, so use the original untranslated
     * physical address.
     */
    assert((orig_addr & ~TARGET_PAGE_MASK) == 0);
    *xlat = orig_addr;
    return &d->map.sections[PHYS_SECTION_UNASSIGNED];
}