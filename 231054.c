static void svm_fpu_deactivate(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	set_exception_intercept(svm, NM_VECTOR);
	update_cr0_intercept(svm);
}