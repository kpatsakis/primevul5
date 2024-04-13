void tipc_crypto_rekeying_sched(struct tipc_crypto *tx, bool changed,
				u32 new_intv)
{
	unsigned long delay;
	bool now = false;

	if (changed) {
		if (new_intv == TIPC_REKEYING_NOW)
			now = true;
		else
			tx->rekeying_intv = new_intv;
		cancel_delayed_work_sync(&tx->work);
	}

	if (tx->rekeying_intv || now) {
		delay = (now) ? 0 : tx->rekeying_intv * 60 * 1000;
		queue_delayed_work(tx->wq, &tx->work, msecs_to_jiffies(delay));
	}
}