static inline int kvm_vcpu_exiting_guest_mode(struct kvm_vcpu *vcpu)
{
	/*
	 * The memory barrier ensures a previous write to vcpu->requests cannot
	 * be reordered with the read of vcpu->mode.  It pairs with the general
	 * memory barrier following the write of vcpu->mode in VCPU RUN.
	 */
	smp_mb__before_atomic();
	return cmpxchg(&vcpu->mode, IN_GUEST_MODE, EXITING_GUEST_MODE);
}