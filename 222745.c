void kvm_arch_sync_dirty_log(struct kvm *kvm, struct kvm_memory_slot *memslot)
{
	int i;
	gfn_t cur_gfn, last_gfn;
	unsigned long gaddr, vmaddr;
	struct gmap *gmap = kvm->arch.gmap;
	DECLARE_BITMAP(bitmap, _PAGE_ENTRIES);

	/* Loop over all guest segments */
	cur_gfn = memslot->base_gfn;
	last_gfn = memslot->base_gfn + memslot->npages;
	for (; cur_gfn <= last_gfn; cur_gfn += _PAGE_ENTRIES) {
		gaddr = gfn_to_gpa(cur_gfn);
		vmaddr = gfn_to_hva_memslot(memslot, cur_gfn);
		if (kvm_is_error_hva(vmaddr))
			continue;

		bitmap_zero(bitmap, _PAGE_ENTRIES);
		gmap_sync_dirty_log_pmd(gmap, bitmap, gaddr, vmaddr);
		for (i = 0; i < _PAGE_ENTRIES; i++) {
			if (test_bit(i, bitmap))
				mark_page_dirty(kvm, cur_gfn + i);
		}

		if (fatal_signal_pending(current))
			return;
		cond_resched();
	}
}