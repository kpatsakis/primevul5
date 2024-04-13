void kvm_arch_vcpu_put(struct kvm_vcpu *vcpu)
{
	vcpu->cpu = -1;
	if (vcpu->arch.cputm_enabled && !is_vcpu_idle(vcpu))
		__stop_cpu_timer_accounting(vcpu);
	kvm_s390_clear_cpuflags(vcpu, CPUSTAT_RUNNING);
	vcpu->arch.enabled_gmap = gmap_get_enabled();
	gmap_disable(vcpu->arch.enabled_gmap);

}