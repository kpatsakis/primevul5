static int kvm_get_stat_per_vcpu(struct kvm *kvm, size_t offset, u64 *val)
{
	int i;
	struct kvm_vcpu *vcpu;

	*val = 0;

	kvm_for_each_vcpu(i, vcpu, kvm)
		*val += *(u64 *)((void *)vcpu + offset);

	return 0;
}