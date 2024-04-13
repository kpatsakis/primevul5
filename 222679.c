void kvm_arch_commit_memory_region(struct kvm *kvm,
				const struct kvm_userspace_memory_region *mem,
				struct kvm_memory_slot *old,
				const struct kvm_memory_slot *new,
				enum kvm_mr_change change)
{
	int rc = 0;

	switch (change) {
	case KVM_MR_DELETE:
		rc = gmap_unmap_segment(kvm->arch.gmap, old->base_gfn * PAGE_SIZE,
					old->npages * PAGE_SIZE);
		break;
	case KVM_MR_MOVE:
		rc = gmap_unmap_segment(kvm->arch.gmap, old->base_gfn * PAGE_SIZE,
					old->npages * PAGE_SIZE);
		if (rc)
			break;
		/* FALLTHROUGH */
	case KVM_MR_CREATE:
		rc = gmap_map_segment(kvm->arch.gmap, mem->userspace_addr,
				      mem->guest_phys_addr, mem->memory_size);
		break;
	case KVM_MR_FLAGS_ONLY:
		break;
	default:
		WARN(1, "Unknown KVM MR CHANGE: %d\n", change);
	}
	if (rc)
		pr_warn("failed to commit memory region\n");
	return;
}