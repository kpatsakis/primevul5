static int kvm_hv_syndbg_complete_userspace(struct kvm_vcpu *vcpu)
{
	struct kvm_hv *hv = to_kvm_hv(vcpu->kvm);

	if (vcpu->run->hyperv.u.syndbg.msr == HV_X64_MSR_SYNDBG_CONTROL)
		hv->hv_syndbg.control.status =
			vcpu->run->hyperv.u.syndbg.status;
	return 1;
}