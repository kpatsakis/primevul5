static int nf_tables_delset(struct sk_buff *skb, const struct nfnl_info *info,
			    const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_set *set;
	struct nft_ctx ctx;
	int err;

	if (nfmsg->nfgen_family == NFPROTO_UNSPEC)
		return -EAFNOSUPPORT;
	if (nla[NFTA_SET_TABLE] == NULL)
		return -EINVAL;

	err = nft_ctx_init_from_setattr(&ctx, net, skb, info->nlh, nla, extack,
					genmask, NETLINK_CB(skb).portid);
	if (err < 0)
		return err;

	if (nla[NFTA_SET_HANDLE]) {
		attr = nla[NFTA_SET_HANDLE];
		set = nft_set_lookup_byhandle(ctx.table, attr, genmask);
	} else {
		attr = nla[NFTA_SET_NAME];
		set = nft_set_lookup(ctx.table, attr, genmask);
	}

	if (IS_ERR(set)) {
		NL_SET_BAD_ATTR(extack, attr);
		return PTR_ERR(set);
	}
	if (set->use ||
	    (info->nlh->nlmsg_flags & NLM_F_NONREC &&
	     atomic_read(&set->nelems) > 0)) {
		NL_SET_BAD_ATTR(extack, attr);
		return -EBUSY;
	}

	return nft_delset(&ctx, set);
}