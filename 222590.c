static void enable_cpu_timer_accounting(struct kvm_vcpu *vcpu)
{
	preempt_disable(); /* protect from TOD sync and vcpu_load/put */
	__enable_cpu_timer_accounting(vcpu);
	preempt_enable();
}