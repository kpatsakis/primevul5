static int rdpmc_interception(struct vcpu_svm *svm)
{
	int err;

	if (!nrips)
		return emulate_on_interception(svm);

	err = kvm_rdpmc(&svm->vcpu);
	return kvm_complete_insn_gp(&svm->vcpu, err);
}