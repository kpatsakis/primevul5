static int invlpg_interception(struct vcpu_svm *svm)
{
	if (!static_cpu_has(X86_FEATURE_DECODEASSISTS))
		return kvm_emulate_instruction(&svm->vcpu, 0);

	kvm_mmu_invlpg(&svm->vcpu, svm->vmcb->control.exit_info_1);
	return kvm_skip_emulated_instruction(&svm->vcpu);
}