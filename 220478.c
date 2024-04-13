static void __nft_release_tables(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_table *table, *nt;

	list_for_each_entry_safe(table, nt, &nft_net->tables, list) {
		if (nft_table_has_owner(table))
			continue;

		__nft_release_table(net, table);
	}
}