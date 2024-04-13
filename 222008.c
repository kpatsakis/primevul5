static u64 kvm_hv_flush_tlb(struct kvm_vcpu *vcpu, u64 ingpa, u16 rep_cnt, bool ex)
{
	struct kvm *kvm = vcpu->kvm;
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);
	struct hv_tlb_flush_ex flush_ex;
	struct hv_tlb_flush flush;
	u64 vp_bitmap[KVM_HV_MAX_SPARSE_VCPU_SET_BITS];
	DECLARE_BITMAP(vcpu_bitmap, KVM_MAX_VCPUS);
	unsigned long *vcpu_mask;
	u64 valid_bank_mask;
	u64 sparse_banks[64];
	int sparse_banks_len;
	bool all_cpus;

	if (!ex) {
		if (unlikely(kvm_read_guest(kvm, ingpa, &flush, sizeof(flush))))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;

		trace_kvm_hv_flush_tlb(flush.processor_mask,
				       flush.address_space, flush.flags);

		valid_bank_mask = BIT_ULL(0);
		sparse_banks[0] = flush.processor_mask;

		/*
		 * Work around possible WS2012 bug: it sends hypercalls
		 * with processor_mask = 0x0 and HV_FLUSH_ALL_PROCESSORS clear,
		 * while also expecting us to flush something and crashing if
		 * we don't. Let's treat processor_mask == 0 same as
		 * HV_FLUSH_ALL_PROCESSORS.
		 */
		all_cpus = (flush.flags & HV_FLUSH_ALL_PROCESSORS) ||
			flush.processor_mask == 0;
	} else {
		if (unlikely(kvm_read_guest(kvm, ingpa, &flush_ex,
					    sizeof(flush_ex))))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;

		trace_kvm_hv_flush_tlb_ex(flush_ex.hv_vp_set.valid_bank_mask,
					  flush_ex.hv_vp_set.format,
					  flush_ex.address_space,
					  flush_ex.flags);

		valid_bank_mask = flush_ex.hv_vp_set.valid_bank_mask;
		all_cpus = flush_ex.hv_vp_set.format !=
			HV_GENERIC_SET_SPARSE_4K;

		sparse_banks_len =
			bitmap_weight((unsigned long *)&valid_bank_mask, 64) *
			sizeof(sparse_banks[0]);

		if (!sparse_banks_len && !all_cpus)
			goto ret_success;

		if (!all_cpus &&
		    kvm_read_guest(kvm,
				   ingpa + offsetof(struct hv_tlb_flush_ex,
						    hv_vp_set.bank_contents),
				   sparse_banks,
				   sparse_banks_len))
			return HV_STATUS_INVALID_HYPERCALL_INPUT;
	}

	cpumask_clear(&hv_vcpu->tlb_flush);

	vcpu_mask = all_cpus ? NULL :
		sparse_set_to_vcpu_mask(kvm, sparse_banks, valid_bank_mask,
					vp_bitmap, vcpu_bitmap);

	/*
	 * vcpu->arch.cr3 may not be up-to-date for running vCPUs so we can't
	 * analyze it here, flush TLB regardless of the specified address space.
	 */
	kvm_make_vcpus_request_mask(kvm, KVM_REQ_HV_TLB_FLUSH,
				    NULL, vcpu_mask, &hv_vcpu->tlb_flush);

ret_success:
	/* We always do full TLB flush, set rep_done = rep_cnt. */
	return (u64)HV_STATUS_SUCCESS |
		((u64)rep_cnt << HV_HYPERCALL_REP_COMP_OFFSET);
}