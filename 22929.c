static char *tipc_crypto_key_dump(struct tipc_crypto *c, char *buf)
{
	struct tipc_key key = c->key;
	struct tipc_aead *aead;
	int k, i = 0;
	char *s;

	for (k = KEY_MIN; k <= KEY_MAX; k++) {
		if (k == KEY_MASTER) {
			if (is_rx(c))
				continue;
			if (time_before(jiffies,
					c->timer2 + TIPC_TX_GRACE_PERIOD))
				s = "ACT";
			else
				s = "PAS";
		} else {
			if (k == key.passive)
				s = "PAS";
			else if (k == key.active)
				s = "ACT";
			else if (k == key.pending)
				s = "PEN";
			else
				s = "-";
		}
		i += scnprintf(buf + i, 200 - i, "\tKey%d: %s", k, s);

		rcu_read_lock();
		aead = rcu_dereference(c->aead[k]);
		if (aead)
			i += scnprintf(buf + i, 200 - i,
				       "{\"0x...%s\", \"%s\"}/%d:%d",
				       aead->hint,
				       (aead->mode == CLUSTER_KEY) ? "c" : "p",
				       atomic_read(&aead->users),
				       refcount_read(&aead->refcnt));
		rcu_read_unlock();
		i += scnprintf(buf + i, 200 - i, "\n");
	}

	if (is_rx(c))
		i += scnprintf(buf + i, 200 - i, "\tPeer RX active: %d\n",
			       atomic_read(&c->peer_rx_active));

	return buf;
}