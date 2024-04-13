static int enable_smi_window(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	if (!gif_set(svm)) {
		if (vgif_enabled(svm))
			set_intercept(svm, INTERCEPT_STGI);
		/* STGI will cause a vm exit */
		return 1;
	}
	return 0;
}