int tipc_node_xmit_skb(struct net *net, struct sk_buff *skb, u32 dnode,
		       u32 selector)
{
	struct sk_buff_head head;

	__skb_queue_head_init(&head);
	__skb_queue_tail(&head, skb);
	tipc_node_xmit(net, &head, dnode, selector);
	return 0;
}