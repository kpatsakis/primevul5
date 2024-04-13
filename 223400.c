void tipc_node_apply_property(struct net *net, struct tipc_bearer *b,
			      int prop)
{
	struct tipc_net *tn = tipc_net(net);
	int bearer_id = b->identity;
	struct sk_buff_head xmitq;
	struct tipc_link_entry *e;
	struct tipc_node *n;

	__skb_queue_head_init(&xmitq);

	rcu_read_lock();

	list_for_each_entry_rcu(n, &tn->node_list, list) {
		tipc_node_write_lock(n);
		e = &n->links[bearer_id];
		if (e->link) {
			if (prop == TIPC_NLA_PROP_TOL)
				tipc_link_set_tolerance(e->link, b->tolerance,
							&xmitq);
			else if (prop == TIPC_NLA_PROP_MTU)
				tipc_link_set_mtu(e->link, b->mtu);

			/* Update MTU for node link entry */
			e->mtu = tipc_link_mss(e->link);
		}

		tipc_node_write_unlock(n);
		tipc_bearer_xmit(net, bearer_id, &xmitq, &e->maddr, NULL);
	}

	rcu_read_unlock();
}