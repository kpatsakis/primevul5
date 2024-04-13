static int stgi_interception(struct vcpu_svm *svm)
{
	int ret;

	if (nested_svm_check_permissions(svm))
		return 1;

	/*
	 * If VGIF is enabled, the STGI intercept is only added to
	 * detect the opening of the SMI/NMI window; remove it now.
	 */
	if (vgif_enabled(svm))
		clr_intercept(svm, INTERCEPT_STGI);

	ret = kvm_skip_emulated_instruction(&svm->vcpu);
	kvm_make_request(KVM_REQ_EVENT, &svm->vcpu);

	enable_gif(svm);

	return ret;
}