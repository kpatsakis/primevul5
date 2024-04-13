static inline void tcp_highest_sack_combine(struct sock *sk,
					    struct sk_buff *old,
					    struct sk_buff *new)
{
	if (tcp_sk(sk)->sacked_out && (old == tcp_sk(sk)->highest_sack))
		tcp_sk(sk)->highest_sack = new;
}