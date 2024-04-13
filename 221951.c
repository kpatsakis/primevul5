static int kvm_hv_msr_get_crash_ctl(struct kvm *kvm, u64 *pdata)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);

	*pdata = hv->hv_crash_ctl;
	return 0;
}