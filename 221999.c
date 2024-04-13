static int kvm_hv_msr_set_crash_data(struct kvm *kvm, u32 index, u64 data)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	size_t size = ARRAY_SIZE(hv->hv_crash_param);

	if (WARN_ON_ONCE(index >= size))
		return -EINVAL;

	hv->hv_crash_param[array_index_nospec(index, size)] = data;
	return 0;
}