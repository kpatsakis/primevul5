static struct kvm_memslots *kvm_dup_memslots(struct kvm_memslots *old,
					     enum kvm_mr_change change)
{
	struct kvm_memslots *slots;
	size_t old_size, new_size;

	old_size = sizeof(struct kvm_memslots) +
		   (sizeof(struct kvm_memory_slot) * old->used_slots);

	if (change == KVM_MR_CREATE)
		new_size = old_size + sizeof(struct kvm_memory_slot);
	else
		new_size = old_size;

	slots = kvzalloc(new_size, GFP_KERNEL_ACCOUNT);
	if (likely(slots))
		memcpy(slots, old, old_size);

	return slots;
}