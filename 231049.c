static void svm_adjust_tsc_offset(struct kvm_vcpu *vcpu, s64 adjustment, bool host)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	WARN_ON(adjustment < 0);
	if (host)
		adjustment = svm_scale_tsc(vcpu, adjustment);

	svm->vmcb->control.tsc_offset += adjustment;
	if (is_guest_mode(vcpu))
		svm->nested.hsave->control.tsc_offset += adjustment;
	else
		trace_kvm_write_tsc_offset(vcpu->vcpu_id,
				     svm->vmcb->control.tsc_offset - adjustment,
				     svm->vmcb->control.tsc_offset);

	mark_dirty(svm->vmcb, VMCB_INTERCEPTS);
}