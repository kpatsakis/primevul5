static inline void adjust_tsc_offset_host(struct kvm_vcpu *vcpu, s64 adjustment)
{
	kvm_x86_ops->adjust_tsc_offset(vcpu, adjustment, true);
}