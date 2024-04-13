void kvm_arch_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{

	gmap_enable(vcpu->arch.enabled_gmap);
	kvm_s390_set_cpuflags(vcpu, CPUSTAT_RUNNING);
	if (vcpu->arch.cputm_enabled && !is_vcpu_idle(vcpu))
		__start_cpu_timer_accounting(vcpu);
	vcpu->cpu = cpu;
}