static int nf_tables_getsetelem(struct sk_buff *skb,
				const struct nfnl_info *info,
				const struct nlattr * const nla[])
{
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_cur(info->net);
	struct net *net = info->net;
	struct nft_set *set;
	struct nlattr *attr;
	struct nft_ctx ctx;
	int rem, err = 0;

	err = nft_ctx_init_from_elemattr(&ctx, net, skb, info->nlh, nla, extack,
					 genmask, NETLINK_CB(skb).portid);
	if (err < 0)
		return err;

	set = nft_set_lookup(ctx.table, nla[NFTA_SET_ELEM_LIST_SET], genmask);
	if (IS_ERR(set))
		return PTR_ERR(set);

	if (info->nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.start = nf_tables_dump_set_start,
			.dump = nf_tables_dump_set,
			.done = nf_tables_dump_set_done,
			.module = THIS_MODULE,
		};
		struct nft_set_dump_ctx dump_ctx = {
			.set = set,
			.ctx = ctx,
		};

		c.data = &dump_ctx;
		return nft_netlink_dump_start_rcu(info->sk, skb, info->nlh, &c);
	}

	if (!nla[NFTA_SET_ELEM_LIST_ELEMENTS])
		return -EINVAL;

	nla_for_each_nested(attr, nla[NFTA_SET_ELEM_LIST_ELEMENTS], rem) {
		err = nft_get_set_elem(&ctx, set, attr);
		if (err < 0)
			break;
	}

	return err;
}