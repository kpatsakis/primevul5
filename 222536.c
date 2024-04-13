int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu)
{
	return kvm_s390_vcpu_has_irq(vcpu, 0);
}