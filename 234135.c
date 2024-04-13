static size_t account(struct entropy_store *r, size_t nbytes, int min,
		      int reserved)
{
	unsigned long flags;

	/* Hold lock while accounting */
	spin_lock_irqsave(&r->lock, flags);

	BUG_ON(r->entropy_count > r->poolinfo->POOLBITS);
	DEBUG_ENT("trying to extract %d bits from %s\n",
		  nbytes * 8, r->name);

	/* Can we pull enough? */
	if (r->entropy_count / 8 < min + reserved) {
		nbytes = 0;
	} else {
		/* If limited, never pull more than available */
		if (r->limit && nbytes + reserved >= r->entropy_count / 8)
			nbytes = r->entropy_count/8 - reserved;

		if (r->entropy_count / 8 >= nbytes + reserved)
			r->entropy_count -= nbytes*8;
		else
			r->entropy_count = reserved;

		if (r->entropy_count < random_write_wakeup_thresh) {
			wake_up_interruptible(&random_write_wait);
			kill_fasync(&fasync, SIGIO, POLL_OUT);
		}
	}

	DEBUG_ENT("debiting %d entropy credits from %s%s\n",
		  nbytes * 8, r->name, r->limit ? "" : " (unlimited)");

	spin_unlock_irqrestore(&r->lock, flags);

	return nbytes;
}