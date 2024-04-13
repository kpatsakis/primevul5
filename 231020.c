static int invlpg_interception(struct vcpu_svm *svm)
{
	if (!static_cpu_has(X86_FEATURE_DECODEASSISTS))
		return emulate_instruction(&svm->vcpu, 0) == EMULATE_DONE;

	kvm_mmu_invlpg(&svm->vcpu, svm->vmcb->control.exit_info_1);
	skip_emulated_instruction(&svm->vcpu);
	return 1;
}