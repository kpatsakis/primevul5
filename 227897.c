static void tcp_timeout_skbs(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *skb;

	if (!tcp_is_fack(tp) || !tcp_head_timedout(sk))
		return;

	skb = tp->scoreboard_skb_hint;
	if (tp->scoreboard_skb_hint == NULL)
		skb = tcp_write_queue_head(sk);

	tcp_for_write_queue_from(skb, sk) {
		if (skb == tcp_send_head(sk))
			break;
		if (!tcp_skb_timedout(sk, skb))
			break;

		tcp_skb_mark_lost(tp, skb);
	}

	tp->scoreboard_skb_hint = skb;

	tcp_verify_left_out(tp);
}