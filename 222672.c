static inline int kvm_vcpu_get_idx(struct kvm_vcpu *vcpu)
{
	return vcpu->vcpu_idx;
}