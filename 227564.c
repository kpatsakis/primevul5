void mark_page_dirty_in_slot(struct kvm *kvm,
			     struct kvm_memory_slot *memslot,
		 	     gfn_t gfn)
{
	if (memslot && kvm_slot_dirty_track_enabled(memslot)) {
		unsigned long rel_gfn = gfn - memslot->base_gfn;
		u32 slot = (memslot->as_id << 16) | memslot->id;

		if (kvm->dirty_ring_size)
			kvm_dirty_ring_push(kvm_dirty_ring_get(kvm),
					    slot, rel_gfn);
		else
			set_bit_le(rel_gfn, memslot->dirty_bitmap);
	}
}