static void nested_prepare_vmcb_control(struct vcpu_svm *svm)
{
	const u32 mask = V_INTR_MASKING_MASK | V_GIF_ENABLE_MASK | V_GIF_MASK;

	if (nested_npt_enabled(svm))
		nested_svm_init_mmu_context(&svm->vcpu);

	svm->vmcb->control.tsc_offset = svm->vcpu.arch.tsc_offset =
		svm->vcpu.arch.l1_tsc_offset + svm->nested.ctl.tsc_offset;

	svm->vmcb->control.int_ctl             =
		(svm->nested.ctl.int_ctl & ~mask) |
		(svm->nested.hsave->control.int_ctl & mask);

	svm->vmcb->control.virt_ext            = svm->nested.ctl.virt_ext;
	svm->vmcb->control.int_vector          = svm->nested.ctl.int_vector;
	svm->vmcb->control.int_state           = svm->nested.ctl.int_state;
	svm->vmcb->control.event_inj           = svm->nested.ctl.event_inj;
	svm->vmcb->control.event_inj_err       = svm->nested.ctl.event_inj_err;

	svm->vmcb->control.pause_filter_count  = svm->nested.ctl.pause_filter_count;
	svm->vmcb->control.pause_filter_thresh = svm->nested.ctl.pause_filter_thresh;

	/* Enter Guest-Mode */
	enter_guest_mode(&svm->vcpu);

	/*
	 * Merge guest and host intercepts - must be called  with vcpu in
	 * guest-mode to take affect here
	 */
	recalc_intercepts(svm);

	vmcb_mark_all_dirty(svm->vmcb);
}