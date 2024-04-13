static inline int tcp_skb_timedout(const struct sock *sk,
				   const struct sk_buff *skb)
{
	return tcp_time_stamp - TCP_SKB_CB(skb)->when > inet_csk(sk)->icsk_rto;
}