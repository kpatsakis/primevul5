struct tipc_crypto *tipc_node_crypto_rx(struct tipc_node *__n)
{
	return (__n) ? __n->crypto_rx : NULL;
}