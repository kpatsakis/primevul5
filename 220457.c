static void __nft_release_hook(struct net *net, struct nft_table *table)
{
	struct nft_chain *chain;

	list_for_each_entry(chain, &table->chains, list)
		nf_tables_unregister_hook(net, table, chain);
}