static void nf_tables_chain_free_chain_rules(struct nft_chain *chain)
{
	struct nft_rule **g0 = rcu_dereference_raw(chain->rules_gen_0);
	struct nft_rule **g1 = rcu_dereference_raw(chain->rules_gen_1);

	if (g0 != g1)
		kvfree(g1);
	kvfree(g0);

	/* should be NULL either via abort or via successful commit */
	WARN_ON_ONCE(chain->rules_next);
	kvfree(chain->rules_next);
}