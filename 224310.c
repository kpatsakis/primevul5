int nested_svm_vmexit(struct vcpu_svm *svm)
{
	int rc;
	struct vmcb *vmcb12;
	struct vmcb *hsave = svm->nested.hsave;
	struct vmcb *vmcb = svm->vmcb;
	struct kvm_host_map map;

	rc = kvm_vcpu_map(&svm->vcpu, gpa_to_gfn(svm->nested.vmcb12_gpa), &map);
	if (rc) {
		if (rc == -EINVAL)
			kvm_inject_gp(&svm->vcpu, 0);
		return 1;
	}

	vmcb12 = map.hva;

	/* Exit Guest-Mode */
	leave_guest_mode(&svm->vcpu);
	svm->nested.vmcb12_gpa = 0;
	WARN_ON_ONCE(svm->nested.nested_run_pending);

	/* in case we halted in L2 */
	svm->vcpu.arch.mp_state = KVM_MP_STATE_RUNNABLE;

	/* Give the current vmcb to the guest */

	vmcb12->save.es     = vmcb->save.es;
	vmcb12->save.cs     = vmcb->save.cs;
	vmcb12->save.ss     = vmcb->save.ss;
	vmcb12->save.ds     = vmcb->save.ds;
	vmcb12->save.gdtr   = vmcb->save.gdtr;
	vmcb12->save.idtr   = vmcb->save.idtr;
	vmcb12->save.efer   = svm->vcpu.arch.efer;
	vmcb12->save.cr0    = kvm_read_cr0(&svm->vcpu);
	vmcb12->save.cr3    = kvm_read_cr3(&svm->vcpu);
	vmcb12->save.cr2    = vmcb->save.cr2;
	vmcb12->save.cr4    = svm->vcpu.arch.cr4;
	vmcb12->save.rflags = kvm_get_rflags(&svm->vcpu);
	vmcb12->save.rip    = kvm_rip_read(&svm->vcpu);
	vmcb12->save.rsp    = kvm_rsp_read(&svm->vcpu);
	vmcb12->save.rax    = kvm_rax_read(&svm->vcpu);
	vmcb12->save.dr7    = vmcb->save.dr7;
	vmcb12->save.dr6    = svm->vcpu.arch.dr6;
	vmcb12->save.cpl    = vmcb->save.cpl;

	vmcb12->control.int_state         = vmcb->control.int_state;
	vmcb12->control.exit_code         = vmcb->control.exit_code;
	vmcb12->control.exit_code_hi      = vmcb->control.exit_code_hi;
	vmcb12->control.exit_info_1       = vmcb->control.exit_info_1;
	vmcb12->control.exit_info_2       = vmcb->control.exit_info_2;

	if (vmcb12->control.exit_code != SVM_EXIT_ERR)
		nested_vmcb_save_pending_event(svm, vmcb12);

	if (svm->nrips_enabled)
		vmcb12->control.next_rip  = vmcb->control.next_rip;

	vmcb12->control.int_ctl           = svm->nested.ctl.int_ctl;
	vmcb12->control.tlb_ctl           = svm->nested.ctl.tlb_ctl;
	vmcb12->control.event_inj         = svm->nested.ctl.event_inj;
	vmcb12->control.event_inj_err     = svm->nested.ctl.event_inj_err;

	vmcb12->control.pause_filter_count =
		svm->vmcb->control.pause_filter_count;
	vmcb12->control.pause_filter_thresh =
		svm->vmcb->control.pause_filter_thresh;

	/* Restore the original control entries */
	copy_vmcb_control_area(&vmcb->control, &hsave->control);

	/* On vmexit the  GIF is set to false */
	svm_set_gif(svm, false);

	svm->vmcb->control.tsc_offset = svm->vcpu.arch.tsc_offset =
		svm->vcpu.arch.l1_tsc_offset;

	svm->nested.ctl.nested_cr3 = 0;

	/* Restore selected save entries */
	svm->vmcb->save.es = hsave->save.es;
	svm->vmcb->save.cs = hsave->save.cs;
	svm->vmcb->save.ss = hsave->save.ss;
	svm->vmcb->save.ds = hsave->save.ds;
	svm->vmcb->save.gdtr = hsave->save.gdtr;
	svm->vmcb->save.idtr = hsave->save.idtr;
	kvm_set_rflags(&svm->vcpu, hsave->save.rflags);
	svm_set_efer(&svm->vcpu, hsave->save.efer);
	svm_set_cr0(&svm->vcpu, hsave->save.cr0 | X86_CR0_PE);
	svm_set_cr4(&svm->vcpu, hsave->save.cr4);
	kvm_rax_write(&svm->vcpu, hsave->save.rax);
	kvm_rsp_write(&svm->vcpu, hsave->save.rsp);
	kvm_rip_write(&svm->vcpu, hsave->save.rip);
	svm->vmcb->save.dr7 = 0;
	svm->vmcb->save.cpl = 0;
	svm->vmcb->control.exit_int_info = 0;

	vmcb_mark_all_dirty(svm->vmcb);

	trace_kvm_nested_vmexit_inject(vmcb12->control.exit_code,
				       vmcb12->control.exit_info_1,
				       vmcb12->control.exit_info_2,
				       vmcb12->control.exit_int_info,
				       vmcb12->control.exit_int_info_err,
				       KVM_ISA_SVM);

	kvm_vcpu_unmap(&svm->vcpu, &map, true);

	nested_svm_uninit_mmu_context(&svm->vcpu);

	rc = nested_svm_load_cr3(&svm->vcpu, hsave->save.cr3, false);
	if (rc)
		return 1;

	if (npt_enabled)
		svm->vmcb->save.cr3 = hsave->save.cr3;

	/*
	 * Drop what we picked up for L2 via svm_complete_interrupts() so it
	 * doesn't end up in L1.
	 */
	svm->vcpu.arch.nmi_injected = false;
	kvm_clear_exception_queue(&svm->vcpu);
	kvm_clear_interrupt_queue(&svm->vcpu);

	return 0;
}