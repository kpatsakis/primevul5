static void tipc_crypto_rcv_complete(struct net *net, struct tipc_aead *aead,
				     struct tipc_bearer *b,
				     struct sk_buff **skb, int err)
{
	struct tipc_skb_cb *skb_cb = TIPC_SKB_CB(*skb);
	struct tipc_crypto *rx = aead->crypto;
	struct tipc_aead *tmp = NULL;
	struct tipc_ehdr *ehdr;
	struct tipc_node *n;

	/* Is this completed by TX? */
	if (unlikely(is_tx(aead->crypto))) {
		rx = skb_cb->tx_clone_ctx.rx;
		pr_debug("TX->RX(%s): err %d, aead %p, skb->next %p, flags %x\n",
			 (rx) ? tipc_node_get_id_str(rx->node) : "-", err, aead,
			 (*skb)->next, skb_cb->flags);
		pr_debug("skb_cb [recurs %d, last %p], tx->aead [%p %p %p]\n",
			 skb_cb->tx_clone_ctx.recurs, skb_cb->tx_clone_ctx.last,
			 aead->crypto->aead[1], aead->crypto->aead[2],
			 aead->crypto->aead[3]);
		if (unlikely(err)) {
			if (err == -EBADMSG && (*skb)->next)
				tipc_rcv(net, (*skb)->next, b);
			goto free_skb;
		}

		if (likely((*skb)->next)) {
			kfree_skb((*skb)->next);
			(*skb)->next = NULL;
		}
		ehdr = (struct tipc_ehdr *)(*skb)->data;
		if (!rx) {
			WARN_ON(ehdr->user != LINK_CONFIG);
			n = tipc_node_create(net, 0, ehdr->id, 0xffffu, 0,
					     true);
			rx = tipc_node_crypto_rx(n);
			if (unlikely(!rx))
				goto free_skb;
		}

		/* Ignore cloning if it was TX master key */
		if (ehdr->tx_key == KEY_MASTER)
			goto rcv;
		if (tipc_aead_clone(&tmp, aead) < 0)
			goto rcv;
		WARN_ON(!refcount_inc_not_zero(&tmp->refcnt));
		if (tipc_crypto_key_attach(rx, tmp, ehdr->tx_key, false) < 0) {
			tipc_aead_free(&tmp->rcu);
			goto rcv;
		}
		tipc_aead_put(aead);
		aead = tmp;
	}

	if (unlikely(err)) {
		tipc_aead_users_dec((struct tipc_aead __force __rcu *)aead, INT_MIN);
		goto free_skb;
	}

	/* Set the RX key's user */
	tipc_aead_users_set((struct tipc_aead __force __rcu *)aead, 1);

	/* Mark this point, RX works */
	rx->timer1 = jiffies;

rcv:
	/* Remove ehdr & auth. tag prior to tipc_rcv() */
	ehdr = (struct tipc_ehdr *)(*skb)->data;

	/* Mark this point, RX passive still works */
	if (rx->key.passive && ehdr->tx_key == rx->key.passive)
		rx->timer2 = jiffies;

	skb_reset_network_header(*skb);
	skb_pull(*skb, tipc_ehdr_size(ehdr));
	pskb_trim(*skb, (*skb)->len - aead->authsize);

	/* Validate TIPCv2 message */
	if (unlikely(!tipc_msg_validate(skb))) {
		pr_err_ratelimited("Packet dropped after decryption!\n");
		goto free_skb;
	}

	/* Ok, everything's fine, try to synch own keys according to peers' */
	tipc_crypto_key_synch(rx, *skb);

	/* Mark skb decrypted */
	skb_cb->decrypted = 1;

	/* Clear clone cxt if any */
	if (likely(!skb_cb->tx_clone_deferred))
		goto exit;
	skb_cb->tx_clone_deferred = 0;
	memset(&skb_cb->tx_clone_ctx, 0, sizeof(skb_cb->tx_clone_ctx));
	goto exit;

free_skb:
	kfree_skb(*skb);
	*skb = NULL;

exit:
	tipc_aead_put(aead);
	if (rx)
		tipc_node_put(rx->node);
}