static void kvm_mmu_notifier_invalidate_range_end(struct mmu_notifier *mn,
					const struct mmu_notifier_range *range)
{
	struct kvm *kvm = mmu_notifier_to_kvm(mn);
	const struct kvm_hva_range hva_range = {
		.start		= range->start,
		.end		= range->end,
		.pte		= __pte(0),
		.handler	= (void *)kvm_null_fn,
		.on_lock	= kvm_dec_notifier_count,
		.flush_on_ret	= false,
		.may_block	= mmu_notifier_range_blockable(range),
	};

	__kvm_handle_hva_range(kvm, &hva_range);

	BUG_ON(kvm->mmu_notifier_count < 0);
}