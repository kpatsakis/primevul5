void tipc_crypto_stop(struct tipc_crypto **crypto)
{
	struct tipc_crypto *c = *crypto;
	u8 k;

	if (!c)
		return;

	/* Flush any queued works & destroy wq */
	if (is_tx(c)) {
		c->rekeying_intv = 0;
		cancel_delayed_work_sync(&c->work);
		destroy_workqueue(c->wq);
	}

	/* Release AEAD keys */
	rcu_read_lock();
	for (k = KEY_MIN; k <= KEY_MAX; k++)
		tipc_aead_put(rcu_dereference(c->aead[k]));
	rcu_read_unlock();
	pr_debug("%s: has been stopped\n", c->name);

	/* Free this crypto statistics */
	free_percpu(c->stats);

	*crypto = NULL;
	kfree_sensitive(c);
}