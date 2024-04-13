int kvm_s390_vcpu_start(struct kvm_vcpu *vcpu)
{
	int i, online_vcpus, r = 0, started_vcpus = 0;

	if (!is_vcpu_stopped(vcpu))
		return 0;

	trace_kvm_s390_vcpu_start_stop(vcpu->vcpu_id, 1);
	/* Only one cpu at a time may enter/leave the STOPPED state. */
	spin_lock(&vcpu->kvm->arch.start_stop_lock);
	online_vcpus = atomic_read(&vcpu->kvm->online_vcpus);

	/* Let's tell the UV that we want to change into the operating state */
	if (kvm_s390_pv_cpu_is_protected(vcpu)) {
		r = kvm_s390_pv_set_cpu_state(vcpu, PV_CPU_STATE_OPR);
		if (r) {
			spin_unlock(&vcpu->kvm->arch.start_stop_lock);
			return r;
		}
	}

	for (i = 0; i < online_vcpus; i++) {
		if (!is_vcpu_stopped(vcpu->kvm->vcpus[i]))
			started_vcpus++;
	}

	if (started_vcpus == 0) {
		/* we're the only active VCPU -> speed it up */
		__enable_ibs_on_vcpu(vcpu);
	} else if (started_vcpus == 1) {
		/*
		 * As we are starting a second VCPU, we have to disable
		 * the IBS facility on all VCPUs to remove potentially
		 * oustanding ENABLE requests.
		 */
		__disable_ibs_on_all_vcpus(vcpu->kvm);
	}

	kvm_s390_clear_cpuflags(vcpu, CPUSTAT_STOPPED);
	/*
	 * The real PSW might have changed due to a RESTART interpreted by the
	 * ultravisor. We block all interrupts and let the next sie exit
	 * refresh our view.
	 */
	if (kvm_s390_pv_cpu_is_protected(vcpu))
		vcpu->arch.sie_block->gpsw.mask &= ~PSW_INT_MASK;
	/*
	 * Another VCPU might have used IBS while we were offline.
	 * Let's play safe and flush the VCPU at startup.
	 */
	kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);
	spin_unlock(&vcpu->kvm->arch.start_stop_lock);
	return 0;
}