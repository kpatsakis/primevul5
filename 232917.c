void skb_consume_udp(struct sock *sk, struct sk_buff *skb, int len)
{
	if (unlikely(READ_ONCE(sk->sk_peek_off) >= 0)) {
		bool slow = lock_sock_fast(sk);

		sk_peek_offset_bwd(sk, len);
		unlock_sock_fast(sk, slow);
	}

	/* In the more common cases we cleared the head states previously,
	 * see __udp_queue_rcv_skb().
	 */
	if (unlikely(udp_skb_has_head_state(skb)))
		skb_release_head_state(skb);
	consume_stateless_skb(skb);
}