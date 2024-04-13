static void tipc_node_free(struct rcu_head *rp)
{
	struct tipc_node *n = container_of(rp, struct tipc_node, rcu);

#ifdef CONFIG_TIPC_CRYPTO
	tipc_crypto_stop(&n->crypto_rx);
#endif
	kfree(n);
}