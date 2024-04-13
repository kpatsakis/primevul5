static int nf_tables_getflowtable(struct sk_buff *skb,
				  const struct nfnl_info *info,
				  const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	u8 genmask = nft_genmask_cur(info->net);
	int family = nfmsg->nfgen_family;
	struct nft_flowtable *flowtable;
	const struct nft_table *table;
	struct net *net = info->net;
	struct sk_buff *skb2;
	int err;

	if (info->nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.start = nf_tables_dump_flowtable_start,
			.dump = nf_tables_dump_flowtable,
			.done = nf_tables_dump_flowtable_done,
			.module = THIS_MODULE,
			.data = (void *)nla,
		};

		return nft_netlink_dump_start_rcu(info->sk, skb, info->nlh, &c);
	}

	if (!nla[NFTA_FLOWTABLE_NAME])
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_FLOWTABLE_TABLE], family,
				 genmask, 0);
	if (IS_ERR(table))
		return PTR_ERR(table);

	flowtable = nft_flowtable_lookup(table, nla[NFTA_FLOWTABLE_NAME],
					 genmask);
	if (IS_ERR(flowtable))
		return PTR_ERR(flowtable);

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!skb2)
		return -ENOMEM;

	err = nf_tables_fill_flowtable_info(skb2, net, NETLINK_CB(skb).portid,
					    info->nlh->nlmsg_seq,
					    NFT_MSG_NEWFLOWTABLE, 0, family,
					    flowtable, &flowtable->hook_list);
	if (err < 0)
		goto err_fill_flowtable_info;

	return nfnetlink_unicast(skb2, net, NETLINK_CB(skb).portid);

err_fill_flowtable_info:
	kfree_skb(skb2);
	return err;
}