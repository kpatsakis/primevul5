static int __tipc_nl_node_set_key(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *attrs[TIPC_NLA_NODE_MAX + 1];
	struct net *net = sock_net(skb->sk);
	struct tipc_crypto *tx = tipc_net(net)->crypto_tx, *c = tx;
	struct tipc_node *n = NULL;
	struct tipc_aead_key *ukey;
	bool rekeying = true, master_key = false;
	u8 *id, *own_id, mode;
	u32 intv = 0;
	int rc = 0;

	if (!info->attrs[TIPC_NLA_NODE])
		return -EINVAL;

	rc = nla_parse_nested(attrs, TIPC_NLA_NODE_MAX,
			      info->attrs[TIPC_NLA_NODE],
			      tipc_nl_node_policy, info->extack);
	if (rc)
		return rc;

	own_id = tipc_own_id(net);
	if (!own_id) {
		GENL_SET_ERR_MSG(info, "not found own node identity (set id?)");
		return -EPERM;
	}

	rc = tipc_nl_retrieve_rekeying(attrs, &intv);
	if (rc == -ENODATA)
		rekeying = false;

	rc = tipc_nl_retrieve_key(attrs, &ukey);
	if (rc == -ENODATA && rekeying)
		goto rekeying;
	else if (rc)
		return rc;

	rc = tipc_aead_key_validate(ukey, info);
	if (rc)
		return rc;

	rc = tipc_nl_retrieve_nodeid(attrs, &id);
	switch (rc) {
	case -ENODATA:
		mode = CLUSTER_KEY;
		master_key = !!(attrs[TIPC_NLA_NODE_KEY_MASTER]);
		break;
	case 0:
		mode = PER_NODE_KEY;
		if (memcmp(id, own_id, NODE_ID_LEN)) {
			n = tipc_node_find_by_id(net, id) ?:
				tipc_node_create(net, 0, id, 0xffffu, 0, true);
			if (unlikely(!n))
				return -ENOMEM;
			c = n->crypto_rx;
		}
		break;
	default:
		return rc;
	}

	/* Initiate the TX/RX key */
	rc = tipc_crypto_key_init(c, ukey, mode, master_key);
	if (n)
		tipc_node_put(n);

	if (unlikely(rc < 0)) {
		GENL_SET_ERR_MSG(info, "unable to initiate or attach new key");
		return rc;
	} else if (c == tx) {
		/* Distribute TX key but not master one */
		if (!master_key && tipc_crypto_key_distr(tx, rc, NULL))
			GENL_SET_ERR_MSG(info, "failed to replicate new key");
rekeying:
		/* Schedule TX rekeying if needed */
		tipc_crypto_rekeying_sched(tx, rekeying, intv);
	}

	return 0;
}