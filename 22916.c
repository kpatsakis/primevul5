static void tipc_crypto_work_tx(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tipc_crypto *tx = container_of(dwork, struct tipc_crypto, work);
	struct tipc_aead_key *skey = NULL;
	struct tipc_key key = tx->key;
	struct tipc_aead *aead;
	int rc = -ENOMEM;

	if (unlikely(key.pending))
		goto resched;

	/* Take current key as a template */
	rcu_read_lock();
	aead = rcu_dereference(tx->aead[key.active ?: KEY_MASTER]);
	if (unlikely(!aead)) {
		rcu_read_unlock();
		/* At least one key should exist for securing */
		return;
	}

	/* Lets duplicate it first */
	skey = kmemdup(aead->key, tipc_aead_key_size(aead->key), GFP_ATOMIC);
	rcu_read_unlock();

	/* Now, generate new key, initiate & distribute it */
	if (likely(skey)) {
		rc = tipc_aead_key_generate(skey) ?:
		     tipc_crypto_key_init(tx, skey, PER_NODE_KEY, false);
		if (likely(rc > 0))
			rc = tipc_crypto_key_distr(tx, rc, NULL);
		kfree_sensitive(skey);
	}

	if (unlikely(rc))
		pr_warn_ratelimited("%s: rekeying returns %d\n", tx->name, rc);

resched:
	/* Re-schedule rekeying if any */
	tipc_crypto_rekeying_sched(tx, false, 0);
}