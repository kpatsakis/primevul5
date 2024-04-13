static bool flatview_access_allowed(MemoryRegion *mr, MemTxAttrs attrs,
                                    hwaddr addr, hwaddr len)
{
    if (likely(!attrs.memory)) {
        return true;
    }
    if (memory_region_is_ram(mr)) {
        return true;
    }
    qemu_log_mask(LOG_GUEST_ERROR,
                  "Invalid access to non-RAM device at "
                  "addr 0x%" HWADDR_PRIX ", size %" HWADDR_PRIu ", "
                  "region '%s'\n", addr, len, memory_region_name(mr));
    return false;
}