static void icpt_operexc_on_all_vcpus(struct kvm *kvm)
{
	unsigned int i;
	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		kvm_s390_sync_request(KVM_REQ_ICPT_OPEREXC, vcpu);
	}
}