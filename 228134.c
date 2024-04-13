static inline struct sk_buff *tcp_send_head(const struct sock *sk)
{
	return sk->sk_send_head;
}