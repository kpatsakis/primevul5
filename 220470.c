static int nf_tables_commit_chain_prepare(struct net *net, struct nft_chain *chain)
{
	struct nft_rule *rule;
	unsigned int alloc = 0;
	int i;

	/* already handled or inactive chain? */
	if (chain->rules_next || !nft_is_active_next(net, chain))
		return 0;

	rule = list_entry(&chain->rules, struct nft_rule, list);
	i = 0;

	list_for_each_entry_continue(rule, &chain->rules, list) {
		if (nft_is_active_next(net, rule))
			alloc++;
	}

	chain->rules_next = nf_tables_chain_alloc_rules(chain, alloc);
	if (!chain->rules_next)
		return -ENOMEM;

	list_for_each_entry_continue(rule, &chain->rules, list) {
		if (nft_is_active_next(net, rule))
			chain->rules_next[i++] = rule;
	}

	chain->rules_next[i] = NULL;
	return 0;
}