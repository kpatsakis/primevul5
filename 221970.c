int kvm_hv_hypercall(struct kvm_vcpu *vcpu)
{
	u64 param, ingpa, outgpa, ret = HV_STATUS_SUCCESS;
	uint16_t code, rep_idx, rep_cnt;
	bool fast, rep;

	/*
	 * hypercall generates UD from non zero cpl and real mode
	 * per HYPER-V spec
	 */
	if (static_call(kvm_x86_get_cpl)(vcpu) != 0 || !is_protmode(vcpu)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

#ifdef CONFIG_X86_64
	if (is_64_bit_mode(vcpu)) {
		param = kvm_rcx_read(vcpu);
		ingpa = kvm_rdx_read(vcpu);
		outgpa = kvm_r8_read(vcpu);
	} else
#endif
	{
		param = ((u64)kvm_rdx_read(vcpu) << 32) |
			(kvm_rax_read(vcpu) & 0xffffffff);
		ingpa = ((u64)kvm_rbx_read(vcpu) << 32) |
			(kvm_rcx_read(vcpu) & 0xffffffff);
		outgpa = ((u64)kvm_rdi_read(vcpu) << 32) |
			(kvm_rsi_read(vcpu) & 0xffffffff);
	}

	code = param & 0xffff;
	fast = !!(param & HV_HYPERCALL_FAST_BIT);
	rep_cnt = (param >> HV_HYPERCALL_REP_COMP_OFFSET) & 0xfff;
	rep_idx = (param >> HV_HYPERCALL_REP_START_OFFSET) & 0xfff;
	rep = !!(rep_cnt || rep_idx);

	trace_kvm_hv_hypercall(code, fast, rep_cnt, rep_idx, ingpa, outgpa);

	switch (code) {
	case HVCALL_NOTIFY_LONG_SPIN_WAIT:
		if (unlikely(rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		kvm_vcpu_on_spin(vcpu, true);
		break;
	case HVCALL_SIGNAL_EVENT:
		if (unlikely(rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hvcall_signal_event(vcpu, fast, ingpa);
		if (ret != HV_STATUS_INVALID_PORT_ID)
			break;
		fallthrough;	/* maybe userspace knows this conn_id */
	case HVCALL_POST_MESSAGE:
		/* don't bother userspace if it has no way to handle it */
		if (unlikely(rep || !to_hv_synic(vcpu)->active)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		vcpu->run->exit_reason = KVM_EXIT_HYPERV;
		vcpu->run->hyperv.type = KVM_EXIT_HYPERV_HCALL;
		vcpu->run->hyperv.u.hcall.input = param;
		vcpu->run->hyperv.u.hcall.params[0] = ingpa;
		vcpu->run->hyperv.u.hcall.params[1] = outgpa;
		vcpu->arch.complete_userspace_io =
				kvm_hv_hypercall_complete_userspace;
		return 0;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST:
		if (unlikely(fast || !rep_cnt || rep_idx)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, ingpa, rep_cnt, false);
		break;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE:
		if (unlikely(fast || rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, ingpa, rep_cnt, false);
		break;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST_EX:
		if (unlikely(fast || !rep_cnt || rep_idx)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, ingpa, rep_cnt, true);
		break;
	case HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE_EX:
		if (unlikely(fast || rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_flush_tlb(vcpu, ingpa, rep_cnt, true);
		break;
	case HVCALL_SEND_IPI:
		if (unlikely(rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_send_ipi(vcpu, ingpa, outgpa, false, fast);
		break;
	case HVCALL_SEND_IPI_EX:
		if (unlikely(fast || rep)) {
			ret = HV_STATUS_INVALID_HYPERCALL_INPUT;
			break;
		}
		ret = kvm_hv_send_ipi(vcpu, ingpa, outgpa, true, false);
		break;
	case HVCALL_POST_DEBUG_DATA:
	case HVCALL_RETRIEVE_DEBUG_DATA:
		if (unlikely(fast)) {
			ret = HV_STATUS_INVALID_PARAMETER;
			break;
		}
		fallthrough;
	case HVCALL_RESET_DEBUG_SESSION: {
		struct kvm_hv_syndbg *syndbg = to_hv_syndbg(vcpu);

		if (!kvm_hv_is_syndbg_enabled(vcpu)) {
			ret = HV_STATUS_INVALID_HYPERCALL_CODE;
			break;
		}

		if (!(syndbg->options & HV_X64_SYNDBG_OPTION_USE_HCALLS)) {
			ret = HV_STATUS_OPERATION_DENIED;
			break;
		}
		vcpu->run->exit_reason = KVM_EXIT_HYPERV;
		vcpu->run->hyperv.type = KVM_EXIT_HYPERV_HCALL;
		vcpu->run->hyperv.u.hcall.input = param;
		vcpu->run->hyperv.u.hcall.params[0] = ingpa;
		vcpu->run->hyperv.u.hcall.params[1] = outgpa;
		vcpu->arch.complete_userspace_io =
				kvm_hv_hypercall_complete_userspace;
		return 0;
	}
	default:
		ret = HV_STATUS_INVALID_HYPERCALL_CODE;
		break;
	}

	return kvm_hv_hypercall_complete(vcpu, ret);
}