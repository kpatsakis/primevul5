static void nft_set_catchall_destroy(const struct nft_ctx *ctx,
				     struct nft_set *set)
{
	struct nft_set_elem_catchall *catchall;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		list_del_rcu(&catchall->list);
		nft_set_elem_destroy(set, catchall->elem, true);
		kfree_rcu(catchall);
	}
}