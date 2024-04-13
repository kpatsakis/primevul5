int tipc_crypto_start(struct tipc_crypto **crypto, struct net *net,
		      struct tipc_node *node)
{
	struct tipc_crypto *c;

	if (*crypto)
		return -EEXIST;

	/* Allocate crypto */
	c = kzalloc(sizeof(*c), GFP_ATOMIC);
	if (!c)
		return -ENOMEM;

	/* Allocate workqueue on TX */
	if (!node) {
		c->wq = alloc_ordered_workqueue("tipc_crypto", 0);
		if (!c->wq) {
			kfree(c);
			return -ENOMEM;
		}
	}

	/* Allocate statistic structure */
	c->stats = alloc_percpu_gfp(struct tipc_crypto_stats, GFP_ATOMIC);
	if (!c->stats) {
		if (c->wq)
			destroy_workqueue(c->wq);
		kfree_sensitive(c);
		return -ENOMEM;
	}

	c->flags = 0;
	c->net = net;
	c->node = node;
	get_random_bytes(&c->key_gen, 2);
	tipc_crypto_key_set_state(c, 0, 0, 0);
	atomic_set(&c->key_distr, 0);
	atomic_set(&c->peer_rx_active, 0);
	atomic64_set(&c->sndnxt, 0);
	c->timer1 = jiffies;
	c->timer2 = jiffies;
	c->rekeying_intv = TIPC_REKEYING_INTV_DEF;
	spin_lock_init(&c->lock);
	scnprintf(c->name, 48, "%s(%s)", (is_rx(c)) ? "RX" : "TX",
		  (is_rx(c)) ? tipc_node_get_id_str(c->node) :
			       tipc_own_id_string(c->net));

	if (is_rx(c))
		INIT_DELAYED_WORK(&c->work, tipc_crypto_work_rx);
	else
		INIT_DELAYED_WORK(&c->work, tipc_crypto_work_tx);

	*crypto = c;
	return 0;
}