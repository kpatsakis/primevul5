static int cpuid_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_cpuid(&svm->vcpu);
}