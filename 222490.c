static inline int kvm_memslot_move_forward(struct kvm_memslots *slots,
					   struct kvm_memory_slot *memslot,
					   int start)
{
	struct kvm_memory_slot *mslots = slots->memslots;
	int i;

	for (i = start; i > 0; i--) {
		if (memslot->base_gfn < mslots[i - 1].base_gfn)
			break;

		WARN_ON_ONCE(memslot->base_gfn == mslots[i - 1].base_gfn);

		/* Shift the next memslot back one and update its index. */
		mslots[i] = mslots[i - 1];
		slots->id_to_index[mslots[i].id] = i;
	}
	return i;
}