static inline int kvm_memslot_move_backward(struct kvm_memslots *slots,
					    struct kvm_memory_slot *memslot)
{
	struct kvm_memory_slot *mslots = slots->memslots;
	int i;

	if (WARN_ON_ONCE(slots->id_to_index[memslot->id] == -1) ||
	    WARN_ON_ONCE(!slots->used_slots))
		return -1;

	/*
	 * Move the target memslot backward in the array by shifting existing
	 * memslots with a higher GFN (than the target memslot) towards the
	 * front of the array.
	 */
	for (i = slots->id_to_index[memslot->id]; i < slots->used_slots - 1; i++) {
		if (memslot->base_gfn > mslots[i + 1].base_gfn)
			break;

		WARN_ON_ONCE(memslot->base_gfn == mslots[i + 1].base_gfn);

		/* Shift the next memslot forward one and update its index. */
		mslots[i] = mslots[i + 1];
		slots->id_to_index[mslots[i].id] = i;
	}
	return i;
}