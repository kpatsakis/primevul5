static u64 kvm_hv_send_ipi(struct kvm_vcpu *vcpu, u64 ingpa, u64 outgpa,
			   bool ex, bool fast)
{
	struct kvm *kvm = vcpu->kvm;
	struct hv_send_ipi_ex send_ipi_ex;
	struct hv_send_ipi send_ipi;
	u64 vp_bitmap[KVM_HV_MAX_SPARSE_VCPU_SET_BITS];
	DECLARE_BITMAP(vcpu_bitmap, KVM_MAX_VCPUS);
	unsigned long *vcpu_mask;
	unsigned long valid_bank_mask;
	u64 sparse_banks[64];
	int sparse_banks_len;
	u32 vector;
	bool all_cpus;

	if (!ex) {
		if (!fast) {
			if (unlikely(kvm_read_guest(kvm, ingpa, &send_ipi,
						    sizeof(send_ipi))))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
			sparse_banks[0] = send_ipi.cpu_mask;
			vector = send_ipi.vector;
		} else {
			/* 'reserved' part of hv_send_ipi should be 0 */
			if (unlikely(ingpa >> 32 != 0))
				return HV_STATUS_INVALID_HYPERCALL_INPUT;
			sparse_banks[0] = outgpa;
			vector = (u32)ingpa;
		}
		all_cpus = false;
		valid_bank_mask = BIT_ULL(0);

		trace_kvm_hv_send_ipi(vector, sparse_banks[0]);
	} else {
		if (unlikely(kvm_read_guest(kvm, ingpa, &send_ipi_ex,
					    sizeof(send_ipi_ex))))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;

		trace_kvm_hv_send_ipi_ex(send_ipi_ex.vector,
					 send_ipi_ex.vp_set.format,
					 send_ipi_ex.vp_set.valid_bank_mask);

		vector = send_ipi_ex.vector;
		valid_bank_mask = send_ipi_ex.vp_set.valid_bank_mask;
		sparse_banks_len = bitmap_weight(&valid_bank_mask, 64) *
			sizeof(sparse_banks[0]);

		all_cpus = send_ipi_ex.vp_set.format == HV_GENERIC_SET_ALL;

		if (!sparse_banks_len)
			goto ret_success;

		if (!all_cpus &&
		    kvm_read_guest(kvm,
				   ingpa + offsetof(struct hv_send_ipi_ex,
						    vp_set.bank_contents),
				   sparse_banks,
				   sparse_banks_len))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;
	}

	if ((vector < HV_IPI_LOW_VECTOR) || (vector > HV_IPI_HIGH_VECTOR))
		return HV_STATUS_INVALID_HYPERCALL_INPUT;

	vcpu_mask = all_cpus ? NULL :
		sparse_set_to_vcpu_mask(kvm, sparse_banks, valid_bank_mask,
					vp_bitmap, vcpu_bitmap);

	kvm_send_ipi_to_many(kvm, vector, vcpu_mask);

ret_success:
	return HV_STATUS_SUCCESS;
}