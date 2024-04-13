static int kvm_clear_dirty_log_protect(struct kvm *kvm,
				       struct kvm_clear_dirty_log *log)
{
	struct kvm_memslots *slots;
	struct kvm_memory_slot *memslot;
	int as_id, id;
	gfn_t offset;
	unsigned long i, n;
	unsigned long *dirty_bitmap;
	unsigned long *dirty_bitmap_buffer;
	bool flush;

	/* Dirty ring tracking is exclusive to dirty log tracking */
	if (kvm->dirty_ring_size)
		return -ENXIO;

	as_id = log->slot >> 16;
	id = (u16)log->slot;
	if (as_id >= KVM_ADDRESS_SPACE_NUM || id >= KVM_USER_MEM_SLOTS)
		return -EINVAL;

	if (log->first_page & 63)
		return -EINVAL;

	slots = __kvm_memslots(kvm, as_id);
	memslot = id_to_memslot(slots, id);
	if (!memslot || !memslot->dirty_bitmap)
		return -ENOENT;

	dirty_bitmap = memslot->dirty_bitmap;

	n = ALIGN(log->num_pages, BITS_PER_LONG) / 8;

	if (log->first_page > memslot->npages ||
	    log->num_pages > memslot->npages - log->first_page ||
	    (log->num_pages < memslot->npages - log->first_page && (log->num_pages & 63)))
	    return -EINVAL;

	kvm_arch_sync_dirty_log(kvm, memslot);

	flush = false;
	dirty_bitmap_buffer = kvm_second_dirty_bitmap(memslot);
	if (copy_from_user(dirty_bitmap_buffer, log->dirty_bitmap, n))
		return -EFAULT;

	KVM_MMU_LOCK(kvm);
	for (offset = log->first_page, i = offset / BITS_PER_LONG,
		 n = DIV_ROUND_UP(log->num_pages, BITS_PER_LONG); n--;
	     i++, offset += BITS_PER_LONG) {
		unsigned long mask = *dirty_bitmap_buffer++;
		atomic_long_t *p = (atomic_long_t *) &dirty_bitmap[i];
		if (!mask)
			continue;

		mask &= atomic_long_fetch_andnot(mask, p);

		/*
		 * mask contains the bits that really have been cleared.  This
		 * never includes any bits beyond the length of the memslot (if
		 * the length is not aligned to 64 pages), therefore it is not
		 * a problem if userspace sets them in log->dirty_bitmap.
		*/
		if (mask) {
			flush = true;
			kvm_arch_mmu_enable_log_dirty_pt_masked(kvm, memslot,
								offset, mask);
		}
	}
	KVM_MMU_UNLOCK(kvm);

	if (flush)
		kvm_arch_flush_remote_tlbs_memslot(kvm, memslot);

	return 0;
}