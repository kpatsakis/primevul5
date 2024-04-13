static int tipc_nl_retrieve_rekeying(struct nlattr **attrs, u32 *intv)
{
	struct nlattr *attr = attrs[TIPC_NLA_NODE_REKEYING];

	if (!attr)
		return -ENODATA;

	*intv = nla_get_u32(attr);
	return 0;
}