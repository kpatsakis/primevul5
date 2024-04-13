static void tcg_register_iommu_notifier(CPUState *cpu,
                                        IOMMUMemoryRegion *iommu_mr,
                                        int iommu_idx)
{
    /* Make sure this CPU has an IOMMU notifier registered for this
     * IOMMU/IOMMU index combination, so that we can flush its TLB
     * when the IOMMU tells us the mappings we've cached have changed.
     */
    MemoryRegion *mr = MEMORY_REGION(iommu_mr);
    TCGIOMMUNotifier *notifier = NULL;
    int i;

    for (i = 0; i < cpu->iommu_notifiers->len; i++) {
        notifier = g_array_index(cpu->iommu_notifiers, TCGIOMMUNotifier *, i);
        if (notifier->mr == mr && notifier->iommu_idx == iommu_idx) {
            break;
        }
    }
    if (i == cpu->iommu_notifiers->len) {
        /* Not found, add a new entry at the end of the array */
        cpu->iommu_notifiers = g_array_set_size(cpu->iommu_notifiers, i + 1);
        notifier = g_new0(TCGIOMMUNotifier, 1);
        g_array_index(cpu->iommu_notifiers, TCGIOMMUNotifier *, i) = notifier;

        notifier->mr = mr;
        notifier->iommu_idx = iommu_idx;
        notifier->cpu = cpu;
        /* Rather than trying to register interest in the specific part
         * of the iommu's address space that we've accessed and then
         * expand it later as subsequent accesses touch more of it, we
         * just register interest in the whole thing, on the assumption
         * that iommu reconfiguration will be rare.
         */
        iommu_notifier_init(&notifier->n,
                            tcg_iommu_unmap_notify,
                            IOMMU_NOTIFIER_UNMAP,
                            0,
                            HWADDR_MAX,
                            iommu_idx);
        memory_region_register_iommu_notifier(notifier->mr, &notifier->n,
                                              &error_fatal);
    }

    if (!notifier->active) {
        notifier->active = true;
    }
}