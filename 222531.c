bool kvm_arch_vcpu_in_kernel(struct kvm_vcpu *vcpu)
{
	return !(vcpu->arch.sie_block->gpsw.mask & PSW_MASK_PSTATE);
}