static void init_std_data(struct entropy_store *r)
{
	ktime_t now;
	unsigned long flags;

	spin_lock_irqsave(&r->lock, flags);
	r->entropy_count = 0;
	spin_unlock_irqrestore(&r->lock, flags);

	now = ktime_get_real();
	mix_pool_bytes(r, &now, sizeof(now));
	mix_pool_bytes(r, utsname(), sizeof(*(utsname())));
}