struct tipc_node *tipc_node_create(struct net *net, u32 addr, u8 *peer_id,
				   u16 capabilities, u32 hash_mixes,
				   bool preliminary)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_node *n, *temp_node;
	struct tipc_link *l;
	unsigned long intv;
	int bearer_id;
	int i;

	spin_lock_bh(&tn->node_list_lock);
	n = tipc_node_find(net, addr) ?:
		tipc_node_find_by_id(net, peer_id);
	if (n) {
		if (!n->preliminary)
			goto update;
		if (preliminary)
			goto exit;
		/* A preliminary node becomes "real" now, refresh its data */
		tipc_node_write_lock(n);
		n->preliminary = false;
		n->addr = addr;
		hlist_del_rcu(&n->hash);
		hlist_add_head_rcu(&n->hash,
				   &tn->node_htable[tipc_hashfn(addr)]);
		list_del_rcu(&n->list);
		list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
			if (n->addr < temp_node->addr)
				break;
		}
		list_add_tail_rcu(&n->list, &temp_node->list);
		tipc_node_write_unlock_fast(n);

update:
		if (n->peer_hash_mix ^ hash_mixes)
			tipc_node_assign_peer_net(n, hash_mixes);
		if (n->capabilities == capabilities)
			goto exit;
		/* Same node may come back with new capabilities */
		tipc_node_write_lock(n);
		n->capabilities = capabilities;
		for (bearer_id = 0; bearer_id < MAX_BEARERS; bearer_id++) {
			l = n->links[bearer_id].link;
			if (l)
				tipc_link_update_caps(l, capabilities);
		}
		tipc_node_write_unlock_fast(n);

		/* Calculate cluster capabilities */
		tn->capabilities = TIPC_NODE_CAPABILITIES;
		list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
			tn->capabilities &= temp_node->capabilities;
		}

		tipc_bcast_toggle_rcast(net,
					(tn->capabilities & TIPC_BCAST_RCAST));

		goto exit;
	}
	n = kzalloc(sizeof(*n), GFP_ATOMIC);
	if (!n) {
		pr_warn("Node creation failed, no memory\n");
		goto exit;
	}
	tipc_nodeid2string(n->peer_id_string, peer_id);
#ifdef CONFIG_TIPC_CRYPTO
	if (unlikely(tipc_crypto_start(&n->crypto_rx, net, n))) {
		pr_warn("Failed to start crypto RX(%s)!\n", n->peer_id_string);
		kfree(n);
		n = NULL;
		goto exit;
	}
#endif
	n->addr = addr;
	n->preliminary = preliminary;
	memcpy(&n->peer_id, peer_id, 16);
	n->net = net;
	n->peer_net = NULL;
	n->peer_hash_mix = 0;
	/* Assign kernel local namespace if exists */
	tipc_node_assign_peer_net(n, hash_mixes);
	n->capabilities = capabilities;
	kref_init(&n->kref);
	rwlock_init(&n->lock);
	INIT_HLIST_NODE(&n->hash);
	INIT_LIST_HEAD(&n->list);
	INIT_LIST_HEAD(&n->publ_list);
	INIT_LIST_HEAD(&n->conn_sks);
	skb_queue_head_init(&n->bc_entry.namedq);
	skb_queue_head_init(&n->bc_entry.inputq1);
	__skb_queue_head_init(&n->bc_entry.arrvq);
	skb_queue_head_init(&n->bc_entry.inputq2);
	for (i = 0; i < MAX_BEARERS; i++)
		spin_lock_init(&n->links[i].lock);
	n->state = SELF_DOWN_PEER_LEAVING;
	n->delete_at = jiffies + msecs_to_jiffies(NODE_CLEANUP_AFTER);
	n->signature = INVALID_NODE_SIG;
	n->active_links[0] = INVALID_BEARER_ID;
	n->active_links[1] = INVALID_BEARER_ID;
	n->bc_entry.link = NULL;
	tipc_node_get(n);
	timer_setup(&n->timer, tipc_node_timeout, 0);
	/* Start a slow timer anyway, crypto needs it */
	n->keepalive_intv = 10000;
	intv = jiffies + msecs_to_jiffies(n->keepalive_intv);
	if (!mod_timer(&n->timer, intv))
		tipc_node_get(n);
	hlist_add_head_rcu(&n->hash, &tn->node_htable[tipc_hashfn(addr)]);
	list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
		if (n->addr < temp_node->addr)
			break;
	}
	list_add_tail_rcu(&n->list, &temp_node->list);
	/* Calculate cluster capabilities */
	tn->capabilities = TIPC_NODE_CAPABILITIES;
	list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
		tn->capabilities &= temp_node->capabilities;
	}
	tipc_bcast_toggle_rcast(net, (tn->capabilities & TIPC_BCAST_RCAST));
	trace_tipc_node_create(n, true, " ");
exit:
	spin_unlock_bh(&tn->node_list_lock);
	return n;
}