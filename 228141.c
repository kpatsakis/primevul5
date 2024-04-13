static inline void tcp_init_send_head(struct sock *sk)
{
	sk->sk_send_head = NULL;
}