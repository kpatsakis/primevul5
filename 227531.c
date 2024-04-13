static int kvm_mmu_notifier_clear_flush_young(struct mmu_notifier *mn,
					      struct mm_struct *mm,
					      unsigned long start,
					      unsigned long end)
{
	trace_kvm_age_hva(start, end);

	return kvm_handle_hva_range(mn, start, end, __pte(0), kvm_age_gfn);
}