static void tipc_node_delete_from_list(struct tipc_node *node)
{
#ifdef CONFIG_TIPC_CRYPTO
	tipc_crypto_key_flush(node->crypto_rx);
#endif
	list_del_rcu(&node->list);
	hlist_del_rcu(&node->hash);
	tipc_node_put(node);
}