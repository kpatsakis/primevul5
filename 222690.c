static int kvm_clear_stat_per_vcpu(struct kvm *kvm, size_t offset)
{
	int i;
	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(i, vcpu, kvm)
		*(u64 *)((void *)vcpu + offset) = 0;

	return 0;
}