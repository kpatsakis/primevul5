static void svm_toggle_avic_for_irq_window(struct kvm_vcpu *vcpu, bool activate)
{
	if (!avic || !lapic_in_kernel(vcpu))
		return;

	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);
	kvm_request_apicv_update(vcpu->kvm, activate,
				 APICV_INHIBIT_REASON_IRQWIN);
	vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
}