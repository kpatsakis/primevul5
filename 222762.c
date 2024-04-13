static int kvm_clear_stat_per_vm(struct kvm *kvm, size_t offset)
{
	*(ulong *)((void *)kvm + offset) = 0;

	return 0;
}