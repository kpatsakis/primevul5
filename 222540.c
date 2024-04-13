static inline bool vcpu_valid_wakeup(struct kvm_vcpu *vcpu)
{
	return vcpu->valid_wakeup;
}