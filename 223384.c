int tipc_nl_node_reset_link_stats(struct sk_buff *skb, struct genl_info *info)
{
	int err;
	char *link_name;
	unsigned int bearer_id;
	struct tipc_link *link;
	struct tipc_node *node;
	struct nlattr *attrs[TIPC_NLA_LINK_MAX + 1];
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = tipc_net(net);
	struct tipc_link_entry *le;

	if (!info->attrs[TIPC_NLA_LINK])
		return -EINVAL;

	err = nla_parse_nested_deprecated(attrs, TIPC_NLA_LINK_MAX,
					  info->attrs[TIPC_NLA_LINK],
					  tipc_nl_link_policy, info->extack);
	if (err)
		return err;

	if (!attrs[TIPC_NLA_LINK_NAME])
		return -EINVAL;

	link_name = nla_data(attrs[TIPC_NLA_LINK_NAME]);

	err = -EINVAL;
	if (!strcmp(link_name, tipc_bclink_name)) {
		err = tipc_bclink_reset_stats(net, tipc_bc_sndlink(net));
		if (err)
			return err;
		return 0;
	} else if (strstr(link_name, tipc_bclink_name)) {
		rcu_read_lock();
		list_for_each_entry_rcu(node, &tn->node_list, list) {
			tipc_node_read_lock(node);
			link = node->bc_entry.link;
			if (link && !strcmp(link_name, tipc_link_name(link))) {
				err = tipc_bclink_reset_stats(net, link);
				tipc_node_read_unlock(node);
				break;
			}
			tipc_node_read_unlock(node);
		}
		rcu_read_unlock();
		return err;
	}

	node = tipc_node_find_by_name(net, link_name, &bearer_id);
	if (!node)
		return -EINVAL;

	le = &node->links[bearer_id];
	tipc_node_read_lock(node);
	spin_lock_bh(&le->lock);
	link = node->links[bearer_id].link;
	if (!link) {
		spin_unlock_bh(&le->lock);
		tipc_node_read_unlock(node);
		return -EINVAL;
	}
	tipc_link_reset_stats(link);
	spin_unlock_bh(&le->lock);
	tipc_node_read_unlock(node);
	return 0;
}