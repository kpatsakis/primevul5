static int tipc_crypto_key_attach(struct tipc_crypto *c,
				  struct tipc_aead *aead, u8 pos,
				  bool master_key)
{
	struct tipc_key key;
	int rc = -EBUSY;
	u8 new_key;

	spin_lock_bh(&c->lock);
	key = c->key;
	if (master_key) {
		new_key = KEY_MASTER;
		goto attach;
	}
	if (key.active && key.passive)
		goto exit;
	if (key.pending) {
		if (tipc_aead_users(c->aead[key.pending]) > 0)
			goto exit;
		/* if (pos): ok with replacing, will be aligned when needed */
		/* Replace it */
		new_key = key.pending;
	} else {
		if (pos) {
			if (key.active && pos != key_next(key.active)) {
				key.passive = pos;
				new_key = pos;
				goto attach;
			} else if (!key.active && !key.passive) {
				key.pending = pos;
				new_key = pos;
				goto attach;
			}
		}
		key.pending = key_next(key.active ?: key.passive);
		new_key = key.pending;
	}

attach:
	aead->crypto = c;
	aead->gen = (is_tx(c)) ? ++c->key_gen : c->key_gen;
	tipc_aead_rcu_replace(c->aead[new_key], aead, &c->lock);
	if (likely(c->key.keys != key.keys))
		tipc_crypto_key_set_state(c, key.passive, key.active,
					  key.pending);
	c->working = 1;
	c->nokey = 0;
	c->key_master |= master_key;
	rc = new_key;

exit:
	spin_unlock_bh(&c->lock);
	return rc;
}