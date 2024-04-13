static __always_inline unsigned long *sparse_set_to_vcpu_mask(
	struct kvm *kvm, u64 *sparse_banks, u64 valid_bank_mask,
	u64 *vp_bitmap, unsigned long *vcpu_bitmap)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	struct kvm_vcpu *vcpu;
	int i, bank, sbank = 0;

	memset(vp_bitmap, 0,
	       KVM_HV_MAX_SPARSE_VCPU_SET_BITS * sizeof(*vp_bitmap));
	for_each_set_bit(bank, (unsigned long *)&valid_bank_mask,
			 KVM_HV_MAX_SPARSE_VCPU_SET_BITS)
		vp_bitmap[bank] = sparse_banks[sbank++];

	if (likely(!atomic_read(&hv->num_mismatched_vp_indexes))) {
		/* for all vcpus vp_index == vcpu_idx */
		return (unsigned long *)vp_bitmap;
	}

	bitmap_zero(vcpu_bitmap, KVM_MAX_VCPUS);
	kvm_for_each_vcpu(i, vcpu, kvm) {
		if (test_bit(kvm_hv_get_vpindex(vcpu), (unsigned long *)vp_bitmap))
			__set_bit(i, vcpu_bitmap);
	}
	return vcpu_bitmap;
}