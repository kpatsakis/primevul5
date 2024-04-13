static int halt_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_halt(&svm->vcpu);
}