static int vmmcall_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_hypercall(&svm->vcpu);
}