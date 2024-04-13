static void svm_refresh_apicv_exec_ctrl(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct vmcb *vmcb = svm->vmcb;
	bool activated = kvm_vcpu_apicv_active(vcpu);

	if (!avic)
		return;

	if (activated) {
		/**
		 * During AVIC temporary deactivation, guest could update
		 * APIC ID, DFR and LDR registers, which would not be trapped
		 * by avic_unaccelerated_access_interception(). In this case,
		 * we need to check and update the AVIC logical APIC ID table
		 * accordingly before re-activating.
		 */
		avic_post_state_restore(vcpu);
		vmcb->control.int_ctl |= AVIC_ENABLE_MASK;
	} else {
		vmcb->control.int_ctl &= ~AVIC_ENABLE_MASK;
	}
	mark_dirty(vmcb, VMCB_AVIC);

	svm_set_pi_irte_mode(vcpu, activated);
}