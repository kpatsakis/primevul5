static int svm_get_msr(struct kvm_vcpu *vcpu, unsigned ecx, u64 *data)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	switch (ecx) {
	case MSR_IA32_TSC: {
		*data = svm->vmcb->control.tsc_offset +
			svm_scale_tsc(vcpu, native_read_tsc());

		break;
	}
	case MSR_STAR:
		*data = svm->vmcb->save.star;
		break;
#ifdef CONFIG_X86_64
	case MSR_LSTAR:
		*data = svm->vmcb->save.lstar;
		break;
	case MSR_CSTAR:
		*data = svm->vmcb->save.cstar;
		break;
	case MSR_KERNEL_GS_BASE:
		*data = svm->vmcb->save.kernel_gs_base;
		break;
	case MSR_SYSCALL_MASK:
		*data = svm->vmcb->save.sfmask;
		break;
#endif
	case MSR_IA32_SYSENTER_CS:
		*data = svm->vmcb->save.sysenter_cs;
		break;
	case MSR_IA32_SYSENTER_EIP:
		*data = svm->sysenter_eip;
		break;
	case MSR_IA32_SYSENTER_ESP:
		*data = svm->sysenter_esp;
		break;
	/*
	 * Nobody will change the following 5 values in the VMCB so we can
	 * safely return them on rdmsr. They will always be 0 until LBRV is
	 * implemented.
	 */
	case MSR_IA32_DEBUGCTLMSR:
		*data = svm->vmcb->save.dbgctl;
		break;
	case MSR_IA32_LASTBRANCHFROMIP:
		*data = svm->vmcb->save.br_from;
		break;
	case MSR_IA32_LASTBRANCHTOIP:
		*data = svm->vmcb->save.br_to;
		break;
	case MSR_IA32_LASTINTFROMIP:
		*data = svm->vmcb->save.last_excp_from;
		break;
	case MSR_IA32_LASTINTTOIP:
		*data = svm->vmcb->save.last_excp_to;
		break;
	case MSR_VM_HSAVE_PA:
		*data = svm->nested.hsave_msr;
		break;
	case MSR_VM_CR:
		*data = svm->nested.vm_cr_msr;
		break;
	case MSR_IA32_UCODE_REV:
		*data = 0x01000065;
		break;
	default:
		return kvm_get_msr_common(vcpu, ecx, data);
	}
	return 0;
}