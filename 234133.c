static void credit_entropy_bits(struct entropy_store *r, int nbits)
{
	unsigned long flags;
	int entropy_count;

	if (!nbits)
		return;

	spin_lock_irqsave(&r->lock, flags);

	DEBUG_ENT("added %d entropy credits to %s\n", nbits, r->name);
	entropy_count = r->entropy_count;
	entropy_count += nbits;
	if (entropy_count < 0) {
		DEBUG_ENT("negative entropy/overflow\n");
		entropy_count = 0;
	} else if (entropy_count > r->poolinfo->POOLBITS)
		entropy_count = r->poolinfo->POOLBITS;
	r->entropy_count = entropy_count;

	/* should we wake readers? */
	if (r == &input_pool && entropy_count >= random_read_wakeup_thresh) {
		wake_up_interruptible(&random_read_wait);
		kill_fasync(&fasync, SIGIO, POLL_IN);
	}
	spin_unlock_irqrestore(&r->lock, flags);
}