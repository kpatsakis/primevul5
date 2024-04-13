static inline void kvm_vcpu_set_in_spin_loop(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.in_spin_loop = val;
}