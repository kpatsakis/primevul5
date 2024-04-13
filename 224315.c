void svm_leave_nested(struct vcpu_svm *svm)
{
	if (is_guest_mode(&svm->vcpu)) {
		struct vmcb *hsave = svm->nested.hsave;
		struct vmcb *vmcb = svm->vmcb;

		svm->nested.nested_run_pending = 0;
		leave_guest_mode(&svm->vcpu);
		copy_vmcb_control_area(&vmcb->control, &hsave->control);
		nested_svm_uninit_mmu_context(&svm->vcpu);
	}

	kvm_clear_request(KVM_REQ_GET_NESTED_STATE_PAGES, &svm->vcpu);
}