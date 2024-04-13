static int kvm_get_stat_per_vm(struct kvm *kvm, size_t offset, u64 *val)
{
	*val = *(ulong *)((void *)kvm + offset);

	return 0;
}