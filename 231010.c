static int pf_interception(struct vcpu_svm *svm)
{
	u64 fault_address = svm->vmcb->control.exit_info_2;
	u32 error_code;
	int r = 1;

	switch (svm->apf_reason) {
	default:
		error_code = svm->vmcb->control.exit_info_1;

		trace_kvm_page_fault(fault_address, error_code);
		if (!npt_enabled && kvm_event_needs_reinjection(&svm->vcpu))
			kvm_mmu_unprotect_page_virt(&svm->vcpu, fault_address);
		r = kvm_mmu_page_fault(&svm->vcpu, fault_address, error_code,
			svm->vmcb->control.insn_bytes,
			svm->vmcb->control.insn_len);
		break;
	case KVM_PV_REASON_PAGE_NOT_PRESENT:
		svm->apf_reason = 0;
		local_irq_disable();
		kvm_async_pf_task_wait(fault_address);
		local_irq_enable();
		break;
	case KVM_PV_REASON_PAGE_READY:
		svm->apf_reason = 0;
		local_irq_disable();
		kvm_async_pf_task_wake(fault_address);
		local_irq_enable();
		break;
	}
	return r;
}