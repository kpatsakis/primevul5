int tipc_nl_node_set_link(struct sk_buff *skb, struct genl_info *info)
{
	int err;
	int res = 0;
	int bearer_id;
	char *name;
	struct tipc_link *link;
	struct tipc_node *node;
	struct sk_buff_head xmitq;
	struct nlattr *attrs[TIPC_NLA_LINK_MAX + 1];
	struct net *net = sock_net(skb->sk);

	__skb_queue_head_init(&xmitq);

	if (!info->attrs[TIPC_NLA_LINK])
		return -EINVAL;

	err = nla_parse_nested_deprecated(attrs, TIPC_NLA_LINK_MAX,
					  info->attrs[TIPC_NLA_LINK],
					  tipc_nl_link_policy, info->extack);
	if (err)
		return err;

	if (!attrs[TIPC_NLA_LINK_NAME])
		return -EINVAL;

	name = nla_data(attrs[TIPC_NLA_LINK_NAME]);

	if (strcmp(name, tipc_bclink_name) == 0)
		return tipc_nl_bc_link_set(net, attrs);

	node = tipc_node_find_by_name(net, name, &bearer_id);
	if (!node)
		return -EINVAL;

	tipc_node_read_lock(node);

	link = node->links[bearer_id].link;
	if (!link) {
		res = -EINVAL;
		goto out;
	}

	if (attrs[TIPC_NLA_LINK_PROP]) {
		struct nlattr *props[TIPC_NLA_PROP_MAX + 1];

		err = tipc_nl_parse_link_prop(attrs[TIPC_NLA_LINK_PROP], props);
		if (err) {
			res = err;
			goto out;
		}

		if (props[TIPC_NLA_PROP_TOL]) {
			u32 tol;

			tol = nla_get_u32(props[TIPC_NLA_PROP_TOL]);
			tipc_link_set_tolerance(link, tol, &xmitq);
		}
		if (props[TIPC_NLA_PROP_PRIO]) {
			u32 prio;

			prio = nla_get_u32(props[TIPC_NLA_PROP_PRIO]);
			tipc_link_set_prio(link, prio, &xmitq);
		}
		if (props[TIPC_NLA_PROP_WIN]) {
			u32 max_win;

			max_win = nla_get_u32(props[TIPC_NLA_PROP_WIN]);
			tipc_link_set_queue_limits(link,
						   tipc_link_min_win(link),
						   max_win);
		}
	}

out:
	tipc_node_read_unlock(node);
	tipc_bearer_xmit(net, bearer_id, &xmitq, &node->links[bearer_id].maddr,
			 NULL);
	return res;
}