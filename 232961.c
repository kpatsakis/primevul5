static void udp_skb_dtor_locked(struct sock *sk, struct sk_buff *skb)
{
	prefetch(&skb->data);
	udp_rmem_release(sk, udp_skb_truesize(skb), 1, true);
}