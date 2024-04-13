static inline bool kvm_request_pending(struct kvm_vcpu *vcpu)
{
	return READ_ONCE(vcpu->requests);
}