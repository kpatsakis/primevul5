static int nf_tables_getchain(struct sk_buff *skb, const struct nfnl_info *info,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_cur(info->net);
	int family = nfmsg->nfgen_family;
	const struct nft_chain *chain;
	struct net *net = info->net;
	struct nft_table *table;
	struct sk_buff *skb2;
	int err;

	if (info->nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.dump = nf_tables_dump_chains,
			.module = THIS_MODULE,
		};

		return nft_netlink_dump_start_rcu(info->sk, skb, info->nlh, &c);
	}

	table = nft_table_lookup(net, nla[NFTA_CHAIN_TABLE], family, genmask, 0);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_TABLE]);
		return PTR_ERR(table);
	}

	chain = nft_chain_lookup(net, table, nla[NFTA_CHAIN_NAME], genmask);
	if (IS_ERR(chain)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_NAME]);
		return PTR_ERR(chain);
	}

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!skb2)
		return -ENOMEM;

	err = nf_tables_fill_chain_info(skb2, net, NETLINK_CB(skb).portid,
					info->nlh->nlmsg_seq, NFT_MSG_NEWCHAIN,
					0, family, table, chain);
	if (err < 0)
		goto err_fill_chain_info;

	return nfnetlink_unicast(skb2, net, NETLINK_CB(skb).portid);

err_fill_chain_info:
	kfree_skb(skb2);
	return err;
}