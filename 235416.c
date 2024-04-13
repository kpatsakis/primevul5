RAMBlock *qemu_ram_alloc(ram_addr_t size, uint32_t ram_flags,
                         MemoryRegion *mr, Error **errp)
{
    assert((ram_flags & ~(RAM_SHARED | RAM_NORESERVE)) == 0);
    return qemu_ram_alloc_internal(size, size, NULL, NULL, ram_flags, mr, errp);
}