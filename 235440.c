void qemu_ram_free(RAMBlock *block)
{
    if (!block) {
        return;
    }

    if (block->host) {
        ram_block_notify_remove(block->host, block->used_length,
                                block->max_length);
    }

    qemu_mutex_lock_ramlist();
    QLIST_REMOVE_RCU(block, next);
    ram_list.mru_block = NULL;
    /* Write list before version */
    smp_wmb();
    ram_list.version++;
    call_rcu(block, reclaim_ramblock, rcu);
    qemu_mutex_unlock_ramlist();
}