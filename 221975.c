static int kvm_hv_get_msr_pw(struct kvm_vcpu *vcpu, u32 msr, u64 *pdata,
			     bool host)
{
	u64 data = 0;
	struct kvm *kvm = vcpu->kvm;
	struct kvm_hv *hv = to_kvm_hv(kvm);

	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
		data = hv->hv_guest_os_id;
		break;
	case HV_X64_MSR_HYPERCALL:
		data = hv->hv_hypercall;
		break;
	case HV_X64_MSR_TIME_REF_COUNT:
		data = get_time_ref_counter(kvm);
		break;
	case HV_X64_MSR_REFERENCE_TSC:
		data = hv->hv_tsc_page;
		break;
	case HV_X64_MSR_CRASH_P0 ... HV_X64_MSR_CRASH_P4:
		return kvm_hv_msr_get_crash_data(kvm,
						 msr - HV_X64_MSR_CRASH_P0,
						 pdata);
	case HV_X64_MSR_CRASH_CTL:
		return kvm_hv_msr_get_crash_ctl(kvm, pdata);
	case HV_X64_MSR_RESET:
		data = 0;
		break;
	case HV_X64_MSR_REENLIGHTENMENT_CONTROL:
		data = hv->hv_reenlightenment_control;
		break;
	case HV_X64_MSR_TSC_EMULATION_CONTROL:
		data = hv->hv_tsc_emulation_control;
		break;
	case HV_X64_MSR_TSC_EMULATION_STATUS:
		data = hv->hv_tsc_emulation_status;
		break;
	case HV_X64_MSR_SYNDBG_OPTIONS:
	case HV_X64_MSR_SYNDBG_CONTROL ... HV_X64_MSR_SYNDBG_PENDING_BUFFER:
		return syndbg_get_msr(vcpu, msr, pdata, host);
	default:
		vcpu_unimpl(vcpu, "Hyper-V unhandled rdmsr: 0x%x\n", msr);
		return 1;
	}

	*pdata = data;
	return 0;
}