static int emulate_on_interception(struct vcpu_svm *svm)
{
	return emulate_instruction(&svm->vcpu, 0) == EMULATE_DONE;
}