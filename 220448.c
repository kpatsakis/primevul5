static void nf_tables_commit_chain_free_rules_old(struct nft_rule **rules)
{
	struct nft_rule **r = rules;
	struct nft_rules_old *old;

	while (*r)
		r++;

	r++;	/* rcu_head is after end marker */
	old = (void *) r;
	old->start = rules;

	call_rcu(&old->h, __nf_tables_commit_chain_free_rules_old);
}