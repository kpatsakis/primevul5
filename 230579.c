static u64 svm_write_l1_tsc_offset(struct kvm_vcpu *vcpu, u64 offset)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	u64 g_tsc_offset = 0;

	if (is_guest_mode(vcpu)) {
		/* Write L1's TSC offset.  */
		g_tsc_offset = svm->vmcb->control.tsc_offset -
			       svm->nested.hsave->control.tsc_offset;
		svm->nested.hsave->control.tsc_offset = offset;
	}

	trace_kvm_write_tsc_offset(vcpu->vcpu_id,
				   svm->vmcb->control.tsc_offset - g_tsc_offset,
				   offset);

	svm->vmcb->control.tsc_offset = offset + g_tsc_offset;

	mark_dirty(svm->vmcb, VMCB_INTERCEPTS);
	return svm->vmcb->control.tsc_offset;
}