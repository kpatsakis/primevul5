static int nested_svm_exit_special(struct vcpu_svm *svm)
{
	u32 exit_code = svm->vmcb->control.exit_code;

	switch (exit_code) {
	case SVM_EXIT_INTR:
	case SVM_EXIT_NMI:
	case SVM_EXIT_EXCP_BASE + MC_VECTOR:
		return NESTED_EXIT_HOST;
	case SVM_EXIT_NPF:
		/* For now we are always handling NPFs when using them */
		if (npt_enabled)
			return NESTED_EXIT_HOST;
		break;
	case SVM_EXIT_EXCP_BASE + PF_VECTOR:
		/* When we're shadowing, trap PFs, but not async PF */
		if (!npt_enabled && svm->vcpu.arch.apf.host_apf_reason == 0)
			return NESTED_EXIT_HOST;
		break;
	default:
		break;
	}

	return NESTED_EXIT_CONTINUE;
}