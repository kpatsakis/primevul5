static int nested_svm_check_exception(struct vcpu_svm *svm, unsigned nr,
				      bool has_error_code, u32 error_code)
{
	int vmexit;

	if (!is_guest_mode(&svm->vcpu))
		return 0;

	vmexit = nested_svm_intercept(svm);
	if (vmexit != NESTED_EXIT_DONE)
		return 0;

	svm->vmcb->control.exit_code = SVM_EXIT_EXCP_BASE + nr;
	svm->vmcb->control.exit_code_hi = 0;
	svm->vmcb->control.exit_info_1 = error_code;

	/*
	 * EXITINFO2 is undefined for all exception intercepts other
	 * than #PF.
	 */
	if (svm->vcpu.arch.exception.nested_apf)
		svm->vmcb->control.exit_info_2 = svm->vcpu.arch.apf.nested_apf_token;
	else if (svm->vcpu.arch.exception.has_payload)
		svm->vmcb->control.exit_info_2 = svm->vcpu.arch.exception.payload;
	else
		svm->vmcb->control.exit_info_2 = svm->vcpu.arch.cr2;

	svm->nested.exit_required = true;
	return vmexit;
}