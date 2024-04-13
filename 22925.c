static bool tipc_crypto_key_try_align(struct tipc_crypto *rx, u8 new_pending)
{
	struct tipc_aead *tmp1, *tmp2 = NULL;
	struct tipc_key key;
	bool aligned = false;
	u8 new_passive = 0;
	int x;

	spin_lock(&rx->lock);
	key = rx->key;
	if (key.pending == new_pending) {
		aligned = true;
		goto exit;
	}
	if (key.active)
		goto exit;
	if (!key.pending)
		goto exit;
	if (tipc_aead_users(rx->aead[key.pending]) > 0)
		goto exit;

	/* Try to "isolate" this pending key first */
	tmp1 = tipc_aead_rcu_ptr(rx->aead[key.pending], &rx->lock);
	if (!refcount_dec_if_one(&tmp1->refcnt))
		goto exit;
	rcu_assign_pointer(rx->aead[key.pending], NULL);

	/* Move passive key if any */
	if (key.passive) {
		tmp2 = rcu_replace_pointer(rx->aead[key.passive], tmp2, lockdep_is_held(&rx->lock));
		x = (key.passive - key.pending + new_pending) % KEY_MAX;
		new_passive = (x <= 0) ? x + KEY_MAX : x;
	}

	/* Re-allocate the key(s) */
	tipc_crypto_key_set_state(rx, new_passive, 0, new_pending);
	rcu_assign_pointer(rx->aead[new_pending], tmp1);
	if (new_passive)
		rcu_assign_pointer(rx->aead[new_passive], tmp2);
	refcount_set(&tmp1->refcnt, 1);
	aligned = true;
	pr_info_ratelimited("%s: key[%d] -> key[%d]\n", rx->name, key.pending,
			    new_pending);

exit:
	spin_unlock(&rx->lock);
	return aligned;
}