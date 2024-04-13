static void svm_fpu_activate(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	clr_exception_intercept(svm, NM_VECTOR);

	svm->vcpu.fpu_active = 1;
	update_cr0_intercept(svm);
}