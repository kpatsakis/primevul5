static struct nft_table *nft_table_lookup(const struct net *net,
					  const struct nlattr *nla,
					  u8 family, u8 genmask, u32 nlpid)
{
	struct nftables_pernet *nft_net;
	struct nft_table *table;

	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	nft_net = nft_pernet(net);
	list_for_each_entry_rcu(table, &nft_net->tables, list,
				lockdep_is_held(&nft_net->commit_mutex)) {
		if (!nla_strcmp(nla, table->name) &&
		    table->family == family &&
		    nft_active_genmask(table, genmask)) {
			if (nft_table_has_owner(table) &&
			    table->nlpid != nlpid)
				return ERR_PTR(-EPERM);

			return table;
		}
	}

	return ERR_PTR(-ENOENT);
}