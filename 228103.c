static inline bool tcp_stream_memory_free(const struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	u32 notsent_bytes = tp->write_seq - tp->snd_nxt;

	return notsent_bytes < tcp_notsent_lowat(tp);
}