static int kvm_hv_msr_set_crash_ctl(struct kvm *kvm, u64 data)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);

	hv->hv_crash_ctl = data & HV_CRASH_CTL_CRASH_NOTIFY;

	return 0;
}