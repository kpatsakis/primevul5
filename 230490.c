static int wbinvd_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_wbinvd(&svm->vcpu);
}