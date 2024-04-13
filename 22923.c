int tipc_crypto_key_distr(struct tipc_crypto *tx, u8 key,
			  struct tipc_node *dest)
{
	struct tipc_aead *aead;
	u32 dnode = tipc_node_get_addr(dest);
	int rc = -ENOKEY;

	if (!sysctl_tipc_key_exchange_enabled)
		return 0;

	if (key) {
		rcu_read_lock();
		aead = tipc_aead_get(tx->aead[key]);
		if (likely(aead)) {
			rc = tipc_crypto_key_xmit(tx->net, aead->key,
						  aead->gen, aead->mode,
						  dnode);
			tipc_aead_put(aead);
		}
		rcu_read_unlock();
	}

	return rc;
}