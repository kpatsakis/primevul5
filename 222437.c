static int kvm_s390_vm_start_migration(struct kvm *kvm)
{
	struct kvm_memory_slot *ms;
	struct kvm_memslots *slots;
	unsigned long ram_pages = 0;
	int slotnr;

	/* migration mode already enabled */
	if (kvm->arch.migration_mode)
		return 0;
	slots = kvm_memslots(kvm);
	if (!slots || !slots->used_slots)
		return -EINVAL;

	if (!kvm->arch.use_cmma) {
		kvm->arch.migration_mode = 1;
		return 0;
	}
	/* mark all the pages in active slots as dirty */
	for (slotnr = 0; slotnr < slots->used_slots; slotnr++) {
		ms = slots->memslots + slotnr;
		if (!ms->dirty_bitmap)
			return -EINVAL;
		/*
		 * The second half of the bitmap is only used on x86,
		 * and would be wasted otherwise, so we put it to good
		 * use here to keep track of the state of the storage
		 * attributes.
		 */
		memset(kvm_second_dirty_bitmap(ms), 0xff, kvm_dirty_bitmap_bytes(ms));
		ram_pages += ms->npages;
	}
	atomic64_set(&kvm->arch.cmma_dirty_pages, ram_pages);
	kvm->arch.migration_mode = 1;
	kvm_s390_sync_request_broadcast(kvm, KVM_REQ_START_MIGRATION);
	return 0;
}