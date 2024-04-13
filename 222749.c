__weak int kvm_arch_mmu_notifier_invalidate_range(struct kvm *kvm,
		unsigned long start, unsigned long end, bool blockable)
{
	return 0;
}