int tipc_nl_peer_rm(struct sk_buff *skb, struct genl_info *info)
{
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct nlattr *attrs[TIPC_NLA_NET_MAX + 1];
	struct tipc_node *peer, *temp_node;
	u8 node_id[NODE_ID_LEN];
	u64 *w0 = (u64 *)&node_id[0];
	u64 *w1 = (u64 *)&node_id[8];
	u32 addr;
	int err;

	/* We identify the peer by its net */
	if (!info->attrs[TIPC_NLA_NET])
		return -EINVAL;

	err = nla_parse_nested_deprecated(attrs, TIPC_NLA_NET_MAX,
					  info->attrs[TIPC_NLA_NET],
					  tipc_nl_net_policy, info->extack);
	if (err)
		return err;

	/* attrs[TIPC_NLA_NET_NODEID] and attrs[TIPC_NLA_NET_ADDR] are
	 * mutually exclusive cases
	 */
	if (attrs[TIPC_NLA_NET_ADDR]) {
		addr = nla_get_u32(attrs[TIPC_NLA_NET_ADDR]);
		if (!addr)
			return -EINVAL;
	}

	if (attrs[TIPC_NLA_NET_NODEID]) {
		if (!attrs[TIPC_NLA_NET_NODEID_W1])
			return -EINVAL;
		*w0 = nla_get_u64(attrs[TIPC_NLA_NET_NODEID]);
		*w1 = nla_get_u64(attrs[TIPC_NLA_NET_NODEID_W1]);
		addr = hash128to32(node_id);
	}

	if (in_own_node(net, addr))
		return -ENOTSUPP;

	spin_lock_bh(&tn->node_list_lock);
	peer = tipc_node_find(net, addr);
	if (!peer) {
		spin_unlock_bh(&tn->node_list_lock);
		return -ENXIO;
	}

	tipc_node_write_lock(peer);
	if (peer->state != SELF_DOWN_PEER_DOWN &&
	    peer->state != SELF_DOWN_PEER_LEAVING) {
		tipc_node_write_unlock(peer);
		err = -EBUSY;
		goto err_out;
	}

	tipc_node_clear_links(peer);
	tipc_node_write_unlock(peer);
	tipc_node_delete(peer);

	/* Calculate cluster capabilities */
	tn->capabilities = TIPC_NODE_CAPABILITIES;
	list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
		tn->capabilities &= temp_node->capabilities;
	}
	tipc_bcast_toggle_rcast(net, (tn->capabilities & TIPC_BCAST_RCAST));
	err = 0;
err_out:
	tipc_node_put(peer);
	spin_unlock_bh(&tn->node_list_lock);

	return err;
}