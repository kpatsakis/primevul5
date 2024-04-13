static struct tipc_aead *tipc_crypto_key_pick_tx(struct tipc_crypto *tx,
						 struct tipc_crypto *rx,
						 struct sk_buff *skb,
						 u8 tx_key)
{
	struct tipc_skb_cb *skb_cb = TIPC_SKB_CB(skb);
	struct tipc_aead *aead = NULL;
	struct tipc_key key = tx->key;
	u8 k, i = 0;

	/* Initialize data if not yet */
	if (!skb_cb->tx_clone_deferred) {
		skb_cb->tx_clone_deferred = 1;
		memset(&skb_cb->tx_clone_ctx, 0, sizeof(skb_cb->tx_clone_ctx));
	}

	skb_cb->tx_clone_ctx.rx = rx;
	if (++skb_cb->tx_clone_ctx.recurs > 2)
		return NULL;

	/* Pick one TX key */
	spin_lock(&tx->lock);
	if (tx_key == KEY_MASTER) {
		aead = tipc_aead_rcu_ptr(tx->aead[KEY_MASTER], &tx->lock);
		goto done;
	}
	do {
		k = (i == 0) ? key.pending :
			((i == 1) ? key.active : key.passive);
		if (!k)
			continue;
		aead = tipc_aead_rcu_ptr(tx->aead[k], &tx->lock);
		if (!aead)
			continue;
		if (aead->mode != CLUSTER_KEY ||
		    aead == skb_cb->tx_clone_ctx.last) {
			aead = NULL;
			continue;
		}
		/* Ok, found one cluster key */
		skb_cb->tx_clone_ctx.last = aead;
		WARN_ON(skb->next);
		skb->next = skb_clone(skb, GFP_ATOMIC);
		if (unlikely(!skb->next))
			pr_warn("Failed to clone skb for next round if any\n");
		break;
	} while (++i < 3);

done:
	if (likely(aead))
		WARN_ON(!refcount_inc_not_zero(&aead->refcnt));
	spin_unlock(&tx->lock);

	return aead;
}