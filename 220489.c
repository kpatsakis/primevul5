static int __nf_tables_dump_rules(struct sk_buff *skb,
				  unsigned int *idx,
				  struct netlink_callback *cb,
				  const struct nft_table *table,
				  const struct nft_chain *chain)
{
	struct net *net = sock_net(skb->sk);
	const struct nft_rule *rule, *prule;
	unsigned int s_idx = cb->args[0];

	prule = NULL;
	list_for_each_entry_rcu(rule, &chain->rules, list) {
		if (!nft_is_active(net, rule))
			goto cont_skip;
		if (*idx < s_idx)
			goto cont;
		if (*idx > s_idx) {
			memset(&cb->args[1], 0,
					sizeof(cb->args) - sizeof(cb->args[0]));
		}
		if (nf_tables_fill_rule_info(skb, net, NETLINK_CB(cb->skb).portid,
					cb->nlh->nlmsg_seq,
					NFT_MSG_NEWRULE,
					NLM_F_MULTI | NLM_F_APPEND,
					table->family,
					table, chain, rule, prule) < 0)
			return 1;

		nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
		prule = rule;
cont_skip:
		(*idx)++;
	}
	return 0;
}