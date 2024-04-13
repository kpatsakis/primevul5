static void ram_block_add(RAMBlock *new_block, Error **errp)
{
    const bool noreserve = qemu_ram_is_noreserve(new_block);
    const bool shared = qemu_ram_is_shared(new_block);
    RAMBlock *block;
    RAMBlock *last_block = NULL;
    ram_addr_t old_ram_size, new_ram_size;
    Error *err = NULL;

    old_ram_size = last_ram_page();

    qemu_mutex_lock_ramlist();
    new_block->offset = find_ram_offset(new_block->max_length);

    if (!new_block->host) {
        if (xen_enabled()) {
            xen_ram_alloc(new_block->offset, new_block->max_length,
                          new_block->mr, &err);
            if (err) {
                error_propagate(errp, err);
                qemu_mutex_unlock_ramlist();
                return;
            }
        } else {
            new_block->host = qemu_anon_ram_alloc(new_block->max_length,
                                                  &new_block->mr->align,
                                                  shared, noreserve);
            if (!new_block->host) {
                error_setg_errno(errp, errno,
                                 "cannot set up guest memory '%s'",
                                 memory_region_name(new_block->mr));
                qemu_mutex_unlock_ramlist();
                return;
            }
            memory_try_enable_merging(new_block->host, new_block->max_length);
        }
    }

    new_ram_size = MAX(old_ram_size,
              (new_block->offset + new_block->max_length) >> TARGET_PAGE_BITS);
    if (new_ram_size > old_ram_size) {
        dirty_memory_extend(old_ram_size, new_ram_size);
    }
    /* Keep the list sorted from biggest to smallest block.  Unlike QTAILQ,
     * QLIST (which has an RCU-friendly variant) does not have insertion at
     * tail, so save the last element in last_block.
     */
    RAMBLOCK_FOREACH(block) {
        last_block = block;
        if (block->max_length < new_block->max_length) {
            break;
        }
    }
    if (block) {
        QLIST_INSERT_BEFORE_RCU(block, new_block, next);
    } else if (last_block) {
        QLIST_INSERT_AFTER_RCU(last_block, new_block, next);
    } else { /* list is empty */
        QLIST_INSERT_HEAD_RCU(&ram_list.blocks, new_block, next);
    }
    ram_list.mru_block = NULL;

    /* Write list before version */
    smp_wmb();
    ram_list.version++;
    qemu_mutex_unlock_ramlist();

    cpu_physical_memory_set_dirty_range(new_block->offset,
                                        new_block->used_length,
                                        DIRTY_CLIENTS_ALL);

    if (new_block->host) {
        qemu_ram_setup_dump(new_block->host, new_block->max_length);
        qemu_madvise(new_block->host, new_block->max_length, QEMU_MADV_HUGEPAGE);
        /*
         * MADV_DONTFORK is also needed by KVM in absence of synchronous MMU
         * Configure it unless the machine is a qtest server, in which case
         * KVM is not used and it may be forked (eg for fuzzing purposes).
         */
        if (!qtest_enabled()) {
            qemu_madvise(new_block->host, new_block->max_length,
                         QEMU_MADV_DONTFORK);
        }
        ram_block_notify_add(new_block->host, new_block->used_length,
                             new_block->max_length);
    }
}