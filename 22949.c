static void tipc_crypto_key_synch(struct tipc_crypto *rx, struct sk_buff *skb)
{
	struct tipc_ehdr *ehdr = (struct tipc_ehdr *)skb_network_header(skb);
	struct tipc_crypto *tx = tipc_net(rx->net)->crypto_tx;
	struct tipc_msg *hdr = buf_msg(skb);
	u32 self = tipc_own_addr(rx->net);
	u8 cur, new;
	unsigned long delay;

	/* Update RX 'key_master' flag according to peer, also mark "legacy" if
	 * a peer has no master key.
	 */
	rx->key_master = ehdr->master_key;
	if (!rx->key_master)
		tx->legacy_user = 1;

	/* For later cases, apply only if message is destined to this node */
	if (!ehdr->destined || msg_short(hdr) || msg_destnode(hdr) != self)
		return;

	/* Case 1: Peer has no keys, let's make master key take over */
	if (ehdr->rx_nokey) {
		/* Set or extend grace period */
		tx->timer2 = jiffies;
		/* Schedule key distributing for the peer if not yet */
		if (tx->key.keys &&
		    !atomic_cmpxchg(&rx->key_distr, 0, KEY_DISTR_SCHED)) {
			get_random_bytes(&delay, 2);
			delay %= 5;
			delay = msecs_to_jiffies(500 * ++delay);
			if (queue_delayed_work(tx->wq, &rx->work, delay))
				tipc_node_get(rx->node);
		}
	} else {
		/* Cancel a pending key distributing if any */
		atomic_xchg(&rx->key_distr, 0);
	}

	/* Case 2: Peer RX active key has changed, let's update own TX users */
	cur = atomic_read(&rx->peer_rx_active);
	new = ehdr->rx_key_active;
	if (tx->key.keys &&
	    cur != new &&
	    atomic_cmpxchg(&rx->peer_rx_active, cur, new) == cur) {
		if (new)
			tipc_aead_users_inc(tx->aead[new], INT_MAX);
		if (cur)
			tipc_aead_users_dec(tx->aead[cur], 0);

		atomic64_set(&rx->sndnxt, 0);
		/* Mark the point TX key users changed */
		tx->timer1 = jiffies;

		pr_debug("%s: key users changed %d-- %d++, peer %s\n",
			 tx->name, cur, new, rx->name);
	}
}