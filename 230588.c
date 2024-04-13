static int skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	if (nrips && svm->vmcb->control.next_rip != 0) {
		WARN_ON_ONCE(!static_cpu_has(X86_FEATURE_NRIPS));
		svm->next_rip = svm->vmcb->control.next_rip;
	}

	if (!svm->next_rip) {
		if (!kvm_emulate_instruction(vcpu, EMULTYPE_SKIP))
			return 0;
	} else {
		if (svm->next_rip - kvm_rip_read(vcpu) > MAX_INST_SIZE)
			pr_err("%s: ip 0x%lx next 0x%llx\n",
			       __func__, kvm_rip_read(vcpu), svm->next_rip);
		kvm_rip_write(vcpu, svm->next_rip);
	}
	svm_set_interrupt_shadow(vcpu, 0);

	return 1;
}