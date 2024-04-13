static int nm_interception(struct vcpu_svm *svm)
{
	svm_fpu_activate(&svm->vcpu);
	return 1;
}