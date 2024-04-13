static void __disable_cpu_timer_accounting(struct kvm_vcpu *vcpu)
{
	WARN_ON_ONCE(!vcpu->arch.cputm_enabled);
	__stop_cpu_timer_accounting(vcpu);
	vcpu->arch.cputm_enabled = false;
}