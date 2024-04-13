static struct sk_buff *__first_packet_length(struct sock *sk,
					     struct sk_buff_head *rcvq,
					     int *total)
{
	struct sk_buff *skb;

	while ((skb = skb_peek(rcvq)) != NULL) {
		if (udp_lib_checksum_complete(skb)) {
			__UDP_INC_STATS(sock_net(sk), UDP_MIB_CSUMERRORS,
					IS_UDPLITE(sk));
			__UDP_INC_STATS(sock_net(sk), UDP_MIB_INERRORS,
					IS_UDPLITE(sk));
			atomic_inc(&sk->sk_drops);
			__skb_unlink(skb, rcvq);
			*total += skb->truesize;
			kfree_skb(skb);
		} else {
			/* the csum related bits could be changed, refresh
			 * the scratch area
			 */
			udp_set_dev_scratch(skb);
			break;
		}
	}
	return skb;
}