static int rdmsr_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_rdmsr(&svm->vcpu);
}