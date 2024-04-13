int tipc_crypto_xmit(struct net *net, struct sk_buff **skb,
		     struct tipc_bearer *b, struct tipc_media_addr *dst,
		     struct tipc_node *__dnode)
{
	struct tipc_crypto *__rx = tipc_node_crypto_rx(__dnode);
	struct tipc_crypto *tx = tipc_net(net)->crypto_tx;
	struct tipc_crypto_stats __percpu *stats = tx->stats;
	struct tipc_msg *hdr = buf_msg(*skb);
	struct tipc_key key = tx->key;
	struct tipc_aead *aead = NULL;
	u32 user = msg_user(hdr);
	u32 type = msg_type(hdr);
	int rc = -ENOKEY;
	u8 tx_key = 0;

	/* No encryption? */
	if (!tx->working)
		return 0;

	/* Pending key if peer has active on it or probing time */
	if (unlikely(key.pending)) {
		tx_key = key.pending;
		if (!tx->key_master && !key.active)
			goto encrypt;
		if (__rx && atomic_read(&__rx->peer_rx_active) == tx_key)
			goto encrypt;
		if (TIPC_SKB_CB(*skb)->xmit_type == SKB_PROBING) {
			pr_debug("%s: probing for key[%d]\n", tx->name,
				 key.pending);
			goto encrypt;
		}
		if (user == LINK_CONFIG || user == LINK_PROTOCOL)
			tipc_crypto_clone_msg(net, *skb, b, dst, __dnode,
					      SKB_PROBING);
	}

	/* Master key if this is a *vital* message or in grace period */
	if (tx->key_master) {
		tx_key = KEY_MASTER;
		if (!key.active)
			goto encrypt;
		if (TIPC_SKB_CB(*skb)->xmit_type == SKB_GRACING) {
			pr_debug("%s: gracing for msg (%d %d)\n", tx->name,
				 user, type);
			goto encrypt;
		}
		if (user == LINK_CONFIG ||
		    (user == LINK_PROTOCOL && type == RESET_MSG) ||
		    (user == MSG_CRYPTO && type == KEY_DISTR_MSG) ||
		    time_before(jiffies, tx->timer2 + TIPC_TX_GRACE_PERIOD)) {
			if (__rx && __rx->key_master &&
			    !atomic_read(&__rx->peer_rx_active))
				goto encrypt;
			if (!__rx) {
				if (likely(!tx->legacy_user))
					goto encrypt;
				tipc_crypto_clone_msg(net, *skb, b, dst,
						      __dnode, SKB_GRACING);
			}
		}
	}

	/* Else, use the active key if any */
	if (likely(key.active)) {
		tx_key = key.active;
		goto encrypt;
	}

	goto exit;

encrypt:
	aead = tipc_aead_get(tx->aead[tx_key]);
	if (unlikely(!aead))
		goto exit;
	rc = tipc_ehdr_build(net, aead, tx_key, *skb, __rx);
	if (likely(rc > 0))
		rc = tipc_aead_encrypt(aead, *skb, b, dst, __dnode);

exit:
	switch (rc) {
	case 0:
		this_cpu_inc(stats->stat[STAT_OK]);
		break;
	case -EINPROGRESS:
	case -EBUSY:
		this_cpu_inc(stats->stat[STAT_ASYNC]);
		*skb = NULL;
		return rc;
	default:
		this_cpu_inc(stats->stat[STAT_NOK]);
		if (rc == -ENOKEY)
			this_cpu_inc(stats->stat[STAT_NOKEYS]);
		else if (rc == -EKEYREVOKED)
			this_cpu_inc(stats->stat[STAT_BADKEYS]);
		kfree_skb(*skb);
		*skb = NULL;
		break;
	}

	tipc_aead_put(aead);
	return rc;
}