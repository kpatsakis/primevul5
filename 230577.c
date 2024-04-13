static int svm_deliver_avic_intr(struct kvm_vcpu *vcpu, int vec)
{
	if (!vcpu->arch.apicv_active)
		return -1;

	kvm_lapic_set_irr(vec, vcpu->arch.apic);
	smp_mb__after_atomic();

	if (avic_vcpu_is_running(vcpu)) {
		int cpuid = vcpu->cpu;

		if (cpuid != get_cpu())
			wrmsrl(SVM_AVIC_DOORBELL, kvm_cpu_get_apicid(cpuid));
		put_cpu();
	} else
		kvm_vcpu_wake_up(vcpu);

	return 0;
}