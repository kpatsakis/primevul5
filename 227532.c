static void kvm_mmu_notifier_change_pte(struct mmu_notifier *mn,
					struct mm_struct *mm,
					unsigned long address,
					pte_t pte)
{
	struct kvm *kvm = mmu_notifier_to_kvm(mn);

	trace_kvm_set_spte_hva(address);

	/*
	 * .change_pte() must be surrounded by .invalidate_range_{start,end}(),
	 * and so always runs with an elevated notifier count.  This obviates
	 * the need to bump the sequence count.
	 */
	WARN_ON_ONCE(!kvm->mmu_notifier_count);

	kvm_handle_hva_range(mn, address, address + 1, pte, kvm_set_spte_gfn);
}