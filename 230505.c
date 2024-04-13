static int clgi_interception(struct vcpu_svm *svm)
{
	int ret;

	if (nested_svm_check_permissions(svm))
		return 1;

	ret = kvm_skip_emulated_instruction(&svm->vcpu);

	disable_gif(svm);

	/* After a CLGI no interrupts should come */
	if (!kvm_vcpu_apicv_active(&svm->vcpu)) {
		svm_clear_vintr(svm);
		svm->vmcb->control.int_ctl &= ~V_IRQ_MASK;
		mark_dirty(svm->vmcb, VMCB_INTR);
	}

	return ret;
}