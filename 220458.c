static void nf_tables_commit_chain(struct net *net, struct nft_chain *chain)
{
	struct nft_rule **g0, **g1;
	bool next_genbit;

	next_genbit = nft_gencursor_next(net);

	g0 = rcu_dereference_protected(chain->rules_gen_0,
				       lockdep_commit_lock_is_held(net));
	g1 = rcu_dereference_protected(chain->rules_gen_1,
				       lockdep_commit_lock_is_held(net));

	/* No changes to this chain? */
	if (chain->rules_next == NULL) {
		/* chain had no change in last or next generation */
		if (g0 == g1)
			return;
		/*
		 * chain had no change in this generation; make sure next
		 * one uses same rules as current generation.
		 */
		if (next_genbit) {
			rcu_assign_pointer(chain->rules_gen_1, g0);
			nf_tables_commit_chain_free_rules_old(g1);
		} else {
			rcu_assign_pointer(chain->rules_gen_0, g1);
			nf_tables_commit_chain_free_rules_old(g0);
		}

		return;
	}

	if (next_genbit)
		rcu_assign_pointer(chain->rules_gen_1, chain->rules_next);
	else
		rcu_assign_pointer(chain->rules_gen_0, chain->rules_next);

	chain->rules_next = NULL;

	if (g0 == g1)
		return;

	if (next_genbit)
		nf_tables_commit_chain_free_rules_old(g1);
	else
		nf_tables_commit_chain_free_rules_old(g0);
}