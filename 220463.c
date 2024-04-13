static int nft_rcv_nl_event(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct nftables_pernet *nft_net;
	struct netlink_notify *n = ptr;
	struct nft_table *table, *nt;
	struct net *net = n->net;
	bool release = false;

	if (event != NETLINK_URELEASE || n->protocol != NETLINK_NETFILTER)
		return NOTIFY_DONE;

	nft_net = nft_pernet(net);
	mutex_lock(&nft_net->commit_mutex);
	list_for_each_entry(table, &nft_net->tables, list) {
		if (nft_table_has_owner(table) &&
		    n->portid == table->nlpid) {
			__nft_release_hook(net, table);
			release = true;
		}
	}
	if (release) {
		synchronize_rcu();
		list_for_each_entry_safe(table, nt, &nft_net->tables, list) {
			if (nft_table_has_owner(table) &&
			    n->portid == table->nlpid)
				__nft_release_table(net, table);
		}
	}
	mutex_unlock(&nft_net->commit_mutex);

	return NOTIFY_DONE;
}