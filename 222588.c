static unsigned long kvm_s390_next_dirty_cmma(struct kvm_memslots *slots,
					      unsigned long cur_gfn)
{
	int slotidx = gfn_to_memslot_approx(slots, cur_gfn);
	struct kvm_memory_slot *ms = slots->memslots + slotidx;
	unsigned long ofs = cur_gfn - ms->base_gfn;

	if (ms->base_gfn + ms->npages <= cur_gfn) {
		slotidx--;
		/* If we are above the highest slot, wrap around */
		if (slotidx < 0)
			slotidx = slots->used_slots - 1;

		ms = slots->memslots + slotidx;
		ofs = 0;
	}
	ofs = find_next_bit(kvm_second_dirty_bitmap(ms), ms->npages, ofs);
	while ((slotidx > 0) && (ofs >= ms->npages)) {
		slotidx--;
		ms = slots->memslots + slotidx;
		ofs = find_next_bit(kvm_second_dirty_bitmap(ms), ms->npages, 0);
	}
	return ms->base_gfn + ofs;
}