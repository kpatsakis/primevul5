bool prepare_mmio_access(MemoryRegion *mr)
{
    bool release_lock = false;

    if (!qemu_mutex_iothread_locked()) {
        qemu_mutex_lock_iothread();
        release_lock = true;
    }
    if (mr->flush_coalesced_mmio) {
        qemu_flush_coalesced_mmio_buffer();
    }

    return release_lock;
}