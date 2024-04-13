static inline u32 tcp_rto_min_us(struct sock *sk)
{
	return jiffies_to_usecs(tcp_rto_min(sk));
}