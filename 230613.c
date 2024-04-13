static int nop_interception(struct vcpu_svm *svm)
{
	return kvm_skip_emulated_instruction(&(svm->vcpu));
}