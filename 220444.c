static int nf_tables_getrule(struct sk_buff *skb, const struct nfnl_info *info,
			     const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(info->nlh);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_cur(info->net);
	int family = nfmsg->nfgen_family;
	const struct nft_chain *chain;
	const struct nft_rule *rule;
	struct net *net = info->net;
	struct nft_table *table;
	struct sk_buff *skb2;
	int err;

	if (info->nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.start= nf_tables_dump_rules_start,
			.dump = nf_tables_dump_rules,
			.done = nf_tables_dump_rules_done,
			.module = THIS_MODULE,
			.data = (void *)nla,
		};

		return nft_netlink_dump_start_rcu(info->sk, skb, info->nlh, &c);
	}

	table = nft_table_lookup(net, nla[NFTA_RULE_TABLE], family, genmask, 0);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_TABLE]);
		return PTR_ERR(table);
	}

	chain = nft_chain_lookup(net, table, nla[NFTA_RULE_CHAIN], genmask);
	if (IS_ERR(chain)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_CHAIN]);
		return PTR_ERR(chain);
	}

	rule = nft_rule_lookup(chain, nla[NFTA_RULE_HANDLE]);
	if (IS_ERR(rule)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_HANDLE]);
		return PTR_ERR(rule);
	}

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!skb2)
		return -ENOMEM;

	err = nf_tables_fill_rule_info(skb2, net, NETLINK_CB(skb).portid,
				       info->nlh->nlmsg_seq, NFT_MSG_NEWRULE, 0,
				       family, table, chain, rule, NULL);
	if (err < 0)
		goto err_fill_rule_info;

	return nfnetlink_unicast(skb2, net, NETLINK_CB(skb).portid);

err_fill_rule_info:
	kfree_skb(skb2);
	return err;
}