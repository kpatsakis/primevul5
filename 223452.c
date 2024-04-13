static int tipc_nl_retrieve_nodeid(struct nlattr **attrs, u8 **node_id)
{
	struct nlattr *attr = attrs[TIPC_NLA_NODE_ID];

	if (!attr)
		return -ENODATA;

	if (nla_len(attr) < TIPC_NODEID_LEN)
		return -EINVAL;

	*node_id = (u8 *)nla_data(attr);
	return 0;
}