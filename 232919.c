int ip6_push_pending_frames(struct sock *sk)
{
	struct sk_buff *skb;

	skb = ip6_finish_skb(sk);
	if (!skb)
		return 0;

	return ip6_send_skb(skb);
}