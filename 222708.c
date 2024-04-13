static void __enable_cpu_timer_accounting(struct kvm_vcpu *vcpu)
{
	WARN_ON_ONCE(vcpu->arch.cputm_enabled);
	vcpu->arch.cputm_enabled = true;
	__start_cpu_timer_accounting(vcpu);
}