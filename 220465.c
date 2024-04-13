static int nft_ctx_init_from_setattr(struct nft_ctx *ctx, struct net *net,
				     const struct sk_buff *skb,
				     const struct nlmsghdr *nlh,
				     const struct nlattr * const nla[],
				     struct netlink_ext_ack *extack,
				     u8 genmask, u32 nlpid)
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	int family = nfmsg->nfgen_family;
	struct nft_table *table = NULL;

	if (nla[NFTA_SET_TABLE] != NULL) {
		table = nft_table_lookup(net, nla[NFTA_SET_TABLE], family,
					 genmask, nlpid);
		if (IS_ERR(table)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_SET_TABLE]);
			return PTR_ERR(table);
		}
	}

	nft_ctx_init(ctx, net, skb, nlh, family, table, NULL, nla);
	return 0;
}