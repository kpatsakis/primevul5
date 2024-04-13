int tipc_node_distr_xmit(struct net *net, struct sk_buff_head *xmitq)
{
	struct sk_buff *skb;
	u32 selector, dnode;

	while ((skb = __skb_dequeue(xmitq))) {
		selector = msg_origport(buf_msg(skb));
		dnode = msg_destnode(buf_msg(skb));
		tipc_node_xmit_skb(net, skb, dnode, selector);
	}
	return 0;
}