static int nf_tables_delflowtable(struct sk_buff *skb,
				  const struct nfnl_info *info,
				  const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	int family = nfmsg->nfgen_family;
	struct nft_flowtable *flowtable;
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_table *table;
	struct nft_ctx ctx;

	if (!nla[NFTA_FLOWTABLE_TABLE] ||
	    (!nla[NFTA_FLOWTABLE_NAME] &&
	     !nla[NFTA_FLOWTABLE_HANDLE]))
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_FLOWTABLE_TABLE], family,
				 genmask, NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_FLOWTABLE_TABLE]);
		return PTR_ERR(table);
	}

	if (nla[NFTA_FLOWTABLE_HANDLE]) {
		attr = nla[NFTA_FLOWTABLE_HANDLE];
		flowtable = nft_flowtable_lookup_byhandle(table, attr, genmask);
	} else {
		attr = nla[NFTA_FLOWTABLE_NAME];
		flowtable = nft_flowtable_lookup(table, attr, genmask);
	}

	if (IS_ERR(flowtable)) {
		NL_SET_BAD_ATTR(extack, attr);
		return PTR_ERR(flowtable);
	}

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

	if (nla[NFTA_FLOWTABLE_HOOK])
		return nft_delflowtable_hook(&ctx, flowtable);

	if (flowtable->use > 0) {
		NL_SET_BAD_ATTR(extack, attr);
		return -EBUSY;
	}

	return nft_delflowtable(&ctx, flowtable);
}