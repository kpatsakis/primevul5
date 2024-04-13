static void __tipc_node_link_up(struct tipc_node *n, int bearer_id,
				struct sk_buff_head *xmitq)
{
	int *slot0 = &n->active_links[0];
	int *slot1 = &n->active_links[1];
	struct tipc_link *ol = node_active_link(n, 0);
	struct tipc_link *nl = n->links[bearer_id].link;

	if (!nl || tipc_link_is_up(nl))
		return;

	tipc_link_fsm_evt(nl, LINK_ESTABLISH_EVT);
	if (!tipc_link_is_up(nl))
		return;

	n->working_links++;
	n->action_flags |= TIPC_NOTIFY_LINK_UP;
	n->link_id = tipc_link_id(nl);

	/* Leave room for tunnel header when returning 'mtu' to users: */
	n->links[bearer_id].mtu = tipc_link_mss(nl);

	tipc_bearer_add_dest(n->net, bearer_id, n->addr);
	tipc_bcast_inc_bearer_dst_cnt(n->net, bearer_id);

	pr_debug("Established link <%s> on network plane %c\n",
		 tipc_link_name(nl), tipc_link_plane(nl));
	trace_tipc_node_link_up(n, true, " ");

	/* Ensure that a STATE message goes first */
	tipc_link_build_state_msg(nl, xmitq);

	/* First link? => give it both slots */
	if (!ol) {
		*slot0 = bearer_id;
		*slot1 = bearer_id;
		tipc_node_fsm_evt(n, SELF_ESTABL_CONTACT_EVT);
		n->action_flags |= TIPC_NOTIFY_NODE_UP;
		tipc_link_set_active(nl, true);
		tipc_bcast_add_peer(n->net, nl, xmitq);
		return;
	}

	/* Second link => redistribute slots */
	if (tipc_link_prio(nl) > tipc_link_prio(ol)) {
		pr_debug("Old link <%s> becomes standby\n", tipc_link_name(ol));
		*slot0 = bearer_id;
		*slot1 = bearer_id;
		tipc_link_set_active(nl, true);
		tipc_link_set_active(ol, false);
	} else if (tipc_link_prio(nl) == tipc_link_prio(ol)) {
		tipc_link_set_active(nl, true);
		*slot1 = bearer_id;
	} else {
		pr_debug("New link <%s> is standby\n", tipc_link_name(nl));
	}

	/* Prepare synchronization with first link */
	tipc_link_tnl_prepare(ol, nl, SYNCH_MSG, xmitq);
}