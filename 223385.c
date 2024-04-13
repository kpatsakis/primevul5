struct tipc_crypto *tipc_node_crypto_rx_by_list(struct list_head *pos)
{
	return container_of(pos, struct tipc_node, list)->crypto_rx;
}