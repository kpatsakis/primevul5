static int emulate_on_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_instruction(&svm->vcpu, 0);
}