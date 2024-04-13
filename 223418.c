static int __tipc_nl_node_flush_key(struct sk_buff *skb,
				    struct genl_info *info)
{
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = tipc_net(net);
	struct tipc_node *n;

	tipc_crypto_key_flush(tn->crypto_tx);
	rcu_read_lock();
	list_for_each_entry_rcu(n, &tn->node_list, list)
		tipc_crypto_key_flush(n->crypto_rx);
	rcu_read_unlock();

	return 0;
}