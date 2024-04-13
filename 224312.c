static int svm_check_nested_events(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	bool block_nested_events =
		kvm_event_needs_reinjection(vcpu) || svm->nested.nested_run_pending;
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (lapic_in_kernel(vcpu) &&
	    test_bit(KVM_APIC_INIT, &apic->pending_events)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_init(svm))
			return 0;
		nested_svm_init(svm);
		return 0;
	}

	if (vcpu->arch.exception.pending) {
		if (block_nested_events)
                        return -EBUSY;
		if (!nested_exit_on_exception(svm))
			return 0;
		nested_svm_inject_exception_vmexit(svm);
		return 0;
	}

	if (vcpu->arch.smi_pending && !svm_smi_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_smi(svm))
			return 0;
		nested_svm_smi(svm);
		return 0;
	}

	if (vcpu->arch.nmi_pending && !svm_nmi_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_nmi(svm))
			return 0;
		nested_svm_nmi(svm);
		return 0;
	}

	if (kvm_cpu_has_interrupt(vcpu) && !svm_interrupt_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_intr(svm))
			return 0;
		nested_svm_intr(svm);
		return 0;
	}

	return 0;
}