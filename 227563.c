static int kvm_delete_memslot(struct kvm *kvm,
			      const struct kvm_userspace_memory_region *mem,
			      struct kvm_memory_slot *old, int as_id)
{
	struct kvm_memory_slot new;
	int r;

	if (!old->npages)
		return -EINVAL;

	memset(&new, 0, sizeof(new));
	new.id = old->id;
	/*
	 * This is only for debugging purpose; it should never be referenced
	 * for a removed memslot.
	 */
	new.as_id = as_id;

	r = kvm_set_memslot(kvm, mem, old, &new, as_id, KVM_MR_DELETE);
	if (r)
		return r;

	kvm_free_memslot(kvm, old);
	return 0;
}