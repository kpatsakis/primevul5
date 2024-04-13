static void __disable_ibs_on_all_vcpus(struct kvm *kvm)
{
	unsigned int i;
	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		__disable_ibs_on_vcpu(vcpu);
	}
}