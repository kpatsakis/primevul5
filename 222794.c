static int kvm_set_memslot(struct kvm *kvm,
			   const struct kvm_userspace_memory_region *mem,
			   struct kvm_memory_slot *old,
			   struct kvm_memory_slot *new, int as_id,
			   enum kvm_mr_change change)
{
	struct kvm_memory_slot *slot;
	struct kvm_memslots *slots;
	int r;

	slots = kvm_dup_memslots(__kvm_memslots(kvm, as_id), change);
	if (!slots)
		return -ENOMEM;

	if (change == KVM_MR_DELETE || change == KVM_MR_MOVE) {
		/*
		 * Note, the INVALID flag needs to be in the appropriate entry
		 * in the freshly allocated memslots, not in @old or @new.
		 */
		slot = id_to_memslot(slots, old->id);
		slot->flags |= KVM_MEMSLOT_INVALID;

		/*
		 * We can re-use the old memslots, the only difference from the
		 * newly installed memslots is the invalid flag, which will get
		 * dropped by update_memslots anyway.  We'll also revert to the
		 * old memslots if preparing the new memory region fails.
		 */
		slots = install_new_memslots(kvm, as_id, slots);

		/* From this point no new shadow pages pointing to a deleted,
		 * or moved, memslot will be created.
		 *
		 * validation of sp->gfn happens in:
		 *	- gfn_to_hva (kvm_read_guest, gfn_to_pfn)
		 *	- kvm_is_visible_gfn (mmu_check_root)
		 */
		kvm_arch_flush_shadow_memslot(kvm, slot);
	}

	r = kvm_arch_prepare_memory_region(kvm, new, mem, change);
	if (r)
		goto out_slots;

	update_memslots(slots, new, change);
	slots = install_new_memslots(kvm, as_id, slots);

	kvm_arch_commit_memory_region(kvm, mem, old, new, change);

	kvfree(slots);
	return 0;

out_slots:
	if (change == KVM_MR_DELETE || change == KVM_MR_MOVE)
		slots = install_new_memslots(kvm, as_id, slots);
	kvfree(slots);
	return r;
}