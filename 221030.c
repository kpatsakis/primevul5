static inline bool nvme_addr_is_iomem(NvmeCtrl *n, hwaddr addr)
{
    hwaddr hi, lo;

    /*
     * The purpose of this check is to guard against invalid "local" access to
     * the iomem (i.e. controller registers). Thus, we check against the range
     * covered by the 'bar0' MemoryRegion since that is currently composed of
     * two subregions (the NVMe "MBAR" and the MSI-X table/pba). Note, however,
     * that if the device model is ever changed to allow the CMB to be located
     * in BAR0 as well, then this must be changed.
     */
    lo = n->bar0.addr;
    hi = lo + int128_get64(n->bar0.size);

    return addr >= lo && addr < hi;
}