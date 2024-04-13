static __always_inline int __kvm_handle_hva_range(struct kvm *kvm,
						  const struct kvm_hva_range *range)
{
	bool ret = false, locked = false;
	struct kvm_gfn_range gfn_range;
	struct kvm_memory_slot *slot;
	struct kvm_memslots *slots;
	int i, idx;

	/* A null handler is allowed if and only if on_lock() is provided. */
	if (WARN_ON_ONCE(IS_KVM_NULL_FN(range->on_lock) &&
			 IS_KVM_NULL_FN(range->handler)))
		return 0;

	idx = srcu_read_lock(&kvm->srcu);

	/* The on_lock() path does not yet support lock elision. */
	if (!IS_KVM_NULL_FN(range->on_lock)) {
		locked = true;
		KVM_MMU_LOCK(kvm);

		range->on_lock(kvm, range->start, range->end);

		if (IS_KVM_NULL_FN(range->handler))
			goto out_unlock;
	}

	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++) {
		slots = __kvm_memslots(kvm, i);
		kvm_for_each_memslot(slot, slots) {
			unsigned long hva_start, hva_end;

			hva_start = max(range->start, slot->userspace_addr);
			hva_end = min(range->end, slot->userspace_addr +
						  (slot->npages << PAGE_SHIFT));
			if (hva_start >= hva_end)
				continue;

			/*
			 * To optimize for the likely case where the address
			 * range is covered by zero or one memslots, don't
			 * bother making these conditional (to avoid writes on
			 * the second or later invocation of the handler).
			 */
			gfn_range.pte = range->pte;
			gfn_range.may_block = range->may_block;

			/*
			 * {gfn(page) | page intersects with [hva_start, hva_end)} =
			 * {gfn_start, gfn_start+1, ..., gfn_end-1}.
			 */
			gfn_range.start = hva_to_gfn_memslot(hva_start, slot);
			gfn_range.end = hva_to_gfn_memslot(hva_end + PAGE_SIZE - 1, slot);
			gfn_range.slot = slot;

			if (!locked) {
				locked = true;
				KVM_MMU_LOCK(kvm);
			}
			ret |= range->handler(kvm, &gfn_range);
		}
	}

	if (range->flush_on_ret && (ret || kvm->tlbs_dirty))
		kvm_flush_remote_tlbs(kvm);

out_unlock:
	if (locked)
		KVM_MMU_UNLOCK(kvm);

	srcu_read_unlock(&kvm->srcu, idx);

	/* The notifiers are averse to booleans. :-( */
	return (int)ret;
}