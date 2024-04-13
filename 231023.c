static int nop_interception(struct vcpu_svm *svm)
{
	skip_emulated_instruction(&(svm->vcpu));
	return 1;
}