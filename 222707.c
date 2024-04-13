static inline void kvm_clear_request(int req, struct kvm_vcpu *vcpu)
{
	clear_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}