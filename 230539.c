static int interrupt_window_interception(struct vcpu_svm *svm)
{
	kvm_make_request(KVM_REQ_EVENT, &svm->vcpu);
	svm_clear_vintr(svm);

	/*
	 * For AVIC, the only reason to end up here is ExtINTs.
	 * In this case AVIC was temporarily disabled for
	 * requesting the IRQ window and we have to re-enable it.
	 */
	svm_toggle_avic_for_irq_window(&svm->vcpu, true);

	svm->vmcb->control.int_ctl &= ~V_IRQ_MASK;
	mark_dirty(svm->vmcb, VMCB_INTR);
	++svm->vcpu.stat.irq_window_exits;
	return 1;
}