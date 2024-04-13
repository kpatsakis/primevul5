static void kvm_clock_sync_scb(struct kvm_s390_sie_block *scb, u64 delta)
{
	u8 delta_idx = 0;

	/*
	 * The TOD jumps by delta, we have to compensate this by adding
	 * -delta to the epoch.
	 */
	delta = -delta;

	/* sign-extension - we're adding to signed values below */
	if ((s64)delta < 0)
		delta_idx = -1;

	scb->epoch += delta;
	if (scb->ecd & ECD_MEF) {
		scb->epdx += delta_idx;
		if (scb->epoch < delta)
			scb->epdx += 1;
	}
}