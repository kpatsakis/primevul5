int kvm_arch_prepare_memory_region(struct kvm *kvm,
				   struct kvm_memory_slot *memslot,
				   const struct kvm_userspace_memory_region *mem,
				   enum kvm_mr_change change)
{
	/* A few sanity checks. We can have memory slots which have to be
	   located/ended at a segment boundary (1MB). The memory in userland is
	   ok to be fragmented into various different vmas. It is okay to mmap()
	   and munmap() stuff in this slot after doing this call at any time */

	if (mem->userspace_addr & 0xffffful)
		return -EINVAL;

	if (mem->memory_size & 0xffffful)
		return -EINVAL;

	if (mem->guest_phys_addr + mem->memory_size > kvm->arch.mem_limit)
		return -EINVAL;

	/* When we are protected, we should not change the memory slots */
	if (kvm_s390_pv_get_handle(kvm))
		return -EINVAL;
	return 0;
}