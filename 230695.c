static void svm_handle_exit_irqoff(struct kvm_vcpu *vcpu,
	enum exit_fastpath_completion *exit_fastpath)
{
	if (!is_guest_mode(vcpu) &&
		to_svm(vcpu)->vmcb->control.exit_code == EXIT_REASON_MSR_WRITE)
		*exit_fastpath = handle_fastpath_set_msr_irqoff(vcpu);
}