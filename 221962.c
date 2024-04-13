static int syndbg_get_msr(struct kvm_vcpu *vcpu, u32 msr, u64 *pdata, bool host)
{
	struct kvm_hv_syndbg *syndbg = to_hv_syndbg(vcpu);

	if (!kvm_hv_is_syndbg_enabled(vcpu) && !host)
		return 1;

	switch (msr) {
	case HV_X64_MSR_SYNDBG_CONTROL:
		*pdata = syndbg->control.control;
		break;
	case HV_X64_MSR_SYNDBG_STATUS:
		*pdata = syndbg->control.status;
		break;
	case HV_X64_MSR_SYNDBG_SEND_BUFFER:
		*pdata = syndbg->control.send_page;
		break;
	case HV_X64_MSR_SYNDBG_RECV_BUFFER:
		*pdata = syndbg->control.recv_page;
		break;
	case HV_X64_MSR_SYNDBG_PENDING_BUFFER:
		*pdata = syndbg->control.pending_page;
		break;
	case HV_X64_MSR_SYNDBG_OPTIONS:
		*pdata = syndbg->options;
		break;
	default:
		break;
	}

	trace_kvm_hv_syndbg_get_msr(vcpu->vcpu_id, kvm_hv_get_vpindex(vcpu), msr, *pdata);

	return 0;
}