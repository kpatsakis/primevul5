static int ud_interception(struct vcpu_svm *svm)
{
	return handle_ud(&svm->vcpu);
}