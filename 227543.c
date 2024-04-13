bool __weak kvm_arch_dy_has_pending_interrupt(struct kvm_vcpu *vcpu)
{
	return false;
}