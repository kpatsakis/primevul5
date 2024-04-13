int kvm_s390_vcpu_stop(struct kvm_vcpu *vcpu)
{
	int i, online_vcpus, r = 0, started_vcpus = 0;
	struct kvm_vcpu *started_vcpu = NULL;

	if (is_vcpu_stopped(vcpu))
		return 0;

	trace_kvm_s390_vcpu_start_stop(vcpu->vcpu_id, 0);
	/* Only one cpu at a time may enter/leave the STOPPED state. */
	spin_lock(&vcpu->kvm->arch.start_stop_lock);
	online_vcpus = atomic_read(&vcpu->kvm->online_vcpus);

	/* Let's tell the UV that we want to change into the stopped state */
	if (kvm_s390_pv_cpu_is_protected(vcpu)) {
		r = kvm_s390_pv_set_cpu_state(vcpu, PV_CPU_STATE_STP);
		if (r) {
			spin_unlock(&vcpu->kvm->arch.start_stop_lock);
			return r;
		}
	}

	/* SIGP STOP and SIGP STOP AND STORE STATUS has been fully processed */
	kvm_s390_clear_stop_irq(vcpu);

	kvm_s390_set_cpuflags(vcpu, CPUSTAT_STOPPED);
	__disable_ibs_on_vcpu(vcpu);

	for (i = 0; i < online_vcpus; i++) {
		if (!is_vcpu_stopped(vcpu->kvm->vcpus[i])) {
			started_vcpus++;
			started_vcpu = vcpu->kvm->vcpus[i];
		}
	}

	if (started_vcpus == 1) {
		/*
		 * As we only have one VCPU left, we want to enable the
		 * IBS facility for that VCPU to speed it up.
		 */
		__enable_ibs_on_vcpu(started_vcpu);
	}

	spin_unlock(&vcpu->kvm->arch.start_stop_lock);
	return 0;
}