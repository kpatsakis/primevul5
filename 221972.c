static int kvm_hv_msr_get_crash_data(struct kvm *kvm, u32 index, u64 *pdata)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	size_t size = ARRAY_SIZE(hv->hv_crash_param);

	if (WARN_ON_ONCE(index >= size))
		return -EINVAL;

	*pdata = hv->hv_crash_param[array_index_nospec(index, size)];
	return 0;
}