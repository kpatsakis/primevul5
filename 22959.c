static void tipc_crypto_work_rx(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct tipc_crypto *rx = container_of(dwork, struct tipc_crypto, work);
	struct tipc_crypto *tx = tipc_net(rx->net)->crypto_tx;
	unsigned long delay = msecs_to_jiffies(5000);
	bool resched = false;
	u8 key;
	int rc;

	/* Case 1: Distribute TX key to peer if scheduled */
	if (atomic_cmpxchg(&rx->key_distr,
			   KEY_DISTR_SCHED,
			   KEY_DISTR_COMPL) == KEY_DISTR_SCHED) {
		/* Always pick the newest one for distributing */
		key = tx->key.pending ?: tx->key.active;
		rc = tipc_crypto_key_distr(tx, key, rx->node);
		if (unlikely(rc))
			pr_warn("%s: unable to distr key[%d] to %s, err %d\n",
				tx->name, key, tipc_node_get_id_str(rx->node),
				rc);

		/* Sched for key_distr releasing */
		resched = true;
	} else {
		atomic_cmpxchg(&rx->key_distr, KEY_DISTR_COMPL, 0);
	}

	/* Case 2: Attach a pending received session key from peer if any */
	if (rx->skey) {
		rc = tipc_crypto_key_init(rx, rx->skey, rx->skey_mode, false);
		if (unlikely(rc < 0))
			pr_warn("%s: unable to attach received skey, err %d\n",
				rx->name, rc);
		switch (rc) {
		case -EBUSY:
		case -ENOMEM:
			/* Resched the key attaching */
			resched = true;
			break;
		default:
			synchronize_rcu();
			kfree(rx->skey);
			rx->skey = NULL;
			break;
		}
	}

	if (resched && queue_delayed_work(tx->wq, &rx->work, delay))
		return;

	tipc_node_put(rx->node);
}