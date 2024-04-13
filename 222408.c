static inline void kvm_vcpu_set_dy_eligible(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.dy_eligible = val;
}