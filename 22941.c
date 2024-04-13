int tipc_crypto_rcv(struct net *net, struct tipc_crypto *rx,
		    struct sk_buff **skb, struct tipc_bearer *b)
{
	struct tipc_crypto *tx = tipc_net(net)->crypto_tx;
	struct tipc_crypto_stats __percpu *stats;
	struct tipc_aead *aead = NULL;
	struct tipc_key key;
	int rc = -ENOKEY;
	u8 tx_key, n;

	tx_key = ((struct tipc_ehdr *)(*skb)->data)->tx_key;

	/* New peer?
	 * Let's try with TX key (i.e. cluster mode) & verify the skb first!
	 */
	if (unlikely(!rx || tx_key == KEY_MASTER))
		goto pick_tx;

	/* Pick RX key according to TX key if any */
	key = rx->key;
	if (tx_key == key.active || tx_key == key.pending ||
	    tx_key == key.passive)
		goto decrypt;

	/* Unknown key, let's try to align RX key(s) */
	if (tipc_crypto_key_try_align(rx, tx_key))
		goto decrypt;

pick_tx:
	/* No key suitable? Try to pick one from TX... */
	aead = tipc_crypto_key_pick_tx(tx, rx, *skb, tx_key);
	if (aead)
		goto decrypt;
	goto exit;

decrypt:
	rcu_read_lock();
	if (!aead)
		aead = tipc_aead_get(rx->aead[tx_key]);
	rc = tipc_aead_decrypt(net, aead, *skb, b);
	rcu_read_unlock();

exit:
	stats = ((rx) ?: tx)->stats;
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
		if (rc == -ENOKEY) {
			kfree_skb(*skb);
			*skb = NULL;
			if (rx) {
				/* Mark rx->nokey only if we dont have a
				 * pending received session key, nor a newer
				 * one i.e. in the next slot.
				 */
				n = key_next(tx_key);
				rx->nokey = !(rx->skey ||
					      rcu_access_pointer(rx->aead[n]));
				pr_debug_ratelimited("%s: nokey %d, key %d/%x\n",
						     rx->name, rx->nokey,
						     tx_key, rx->key.keys);
				tipc_node_put(rx->node);
			}
			this_cpu_inc(stats->stat[STAT_NOKEYS]);
			return rc;
		} else if (rc == -EBADMSG) {
			this_cpu_inc(stats->stat[STAT_BADMSGS]);
		}
		break;
	}

	tipc_crypto_rcv_complete(net, aead, b, skb, rc);
	return rc;
}