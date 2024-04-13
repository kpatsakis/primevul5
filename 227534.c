static int kvm_mmu_notifier_test_young(struct mmu_notifier *mn,
				       struct mm_struct *mm,
				       unsigned long address)
{
	trace_kvm_test_age_hva(address);

	return kvm_handle_hva_range_no_flush(mn, address, address + 1,
					     kvm_test_age_gfn);
}