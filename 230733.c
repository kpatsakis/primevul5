static int svm_smi_allowed(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	/* Per APM Vol.2 15.22.2 "Response to SMI" */
	if (!gif_set(svm))
		return 0;

	if (is_guest_mode(&svm->vcpu) &&
	    svm->nested.intercept & (1ULL << INTERCEPT_SMI)) {
		/* TODO: Might need to set exit_info_1 and exit_info_2 here */
		svm->vmcb->control.exit_code = SVM_EXIT_SMI;
		svm->nested.exit_required = true;
		return 0;
	}

	return 1;
}