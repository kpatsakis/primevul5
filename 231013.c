static int rdpmc_interception(struct vcpu_svm *svm)
{
	int err;

	if (!static_cpu_has(X86_FEATURE_NRIPS))
		return emulate_on_interception(svm);

	err = kvm_rdpmc(&svm->vcpu);
	kvm_complete_insn_gp(&svm->vcpu, err);

	return 1;
}