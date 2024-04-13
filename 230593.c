static int wrmsr_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_wrmsr(&svm->vcpu);
}