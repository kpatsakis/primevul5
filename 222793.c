static inline struct kvm_vcpu *kvm_get_vcpu_by_id(struct kvm *kvm, int id)
{
	struct kvm_vcpu *vcpu = NULL;
	int i;

	if (id < 0)
		return NULL;
	if (id < KVM_MAX_VCPUS)
		vcpu = kvm_get_vcpu(kvm, id);
	if (vcpu && vcpu->vcpu_id == id)
		return vcpu;
	kvm_for_each_vcpu(i, vcpu, kvm)
		if (vcpu->vcpu_id == id)
			return vcpu;
	return NULL;
}