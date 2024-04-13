bool __weak kvm_arch_dy_runnable(struct kvm_vcpu *vcpu)
{
	return kvm_arch_vcpu_runnable(vcpu);
}