int tipc_nl_node_dump_link(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct nlattr **attrs = genl_dumpit_info(cb)->attrs;
	struct nlattr *link[TIPC_NLA_LINK_MAX + 1];
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_node *node;
	struct tipc_nl_msg msg;
	u32 prev_node = cb->args[0];
	u32 prev_link = cb->args[1];
	int done = cb->args[2];
	bool bc_link = cb->args[3];
	int err;

	if (done)
		return 0;

	if (!prev_node) {
		/* Check if broadcast-receiver links dumping is needed */
		if (attrs && attrs[TIPC_NLA_LINK]) {
			err = nla_parse_nested_deprecated(link,
							  TIPC_NLA_LINK_MAX,
							  attrs[TIPC_NLA_LINK],
							  tipc_nl_link_policy,
							  NULL);
			if (unlikely(err))
				return err;
			if (unlikely(!link[TIPC_NLA_LINK_BROADCAST]))
				return -EINVAL;
			bc_link = true;
		}
	}

	msg.skb = skb;
	msg.portid = NETLINK_CB(cb->skb).portid;
	msg.seq = cb->nlh->nlmsg_seq;

	rcu_read_lock();
	if (prev_node) {
		node = tipc_node_find(net, prev_node);
		if (!node) {
			/* We never set seq or call nl_dump_check_consistent()
			 * this means that setting prev_seq here will cause the
			 * consistence check to fail in the netlink callback
			 * handler. Resulting in the last NLMSG_DONE message
			 * having the NLM_F_DUMP_INTR flag set.
			 */
			cb->prev_seq = 1;
			goto out;
		}
		tipc_node_put(node);

		list_for_each_entry_continue_rcu(node, &tn->node_list,
						 list) {
			tipc_node_read_lock(node);
			err = __tipc_nl_add_node_links(net, &msg, node,
						       &prev_link, bc_link);
			tipc_node_read_unlock(node);
			if (err)
				goto out;

			prev_node = node->addr;
		}
	} else {
		err = tipc_nl_add_bc_link(net, &msg, tn->bcl);
		if (err)
			goto out;

		list_for_each_entry_rcu(node, &tn->node_list, list) {
			tipc_node_read_lock(node);
			err = __tipc_nl_add_node_links(net, &msg, node,
						       &prev_link, bc_link);
			tipc_node_read_unlock(node);
			if (err)
				goto out;

			prev_node = node->addr;
		}
	}
	done = 1;
out:
	rcu_read_unlock();

	cb->args[0] = prev_node;
	cb->args[1] = prev_link;
	cb->args[2] = done;
	cb->args[3] = bc_link;

	return skb->len;
}