static void tcp_new_space(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (tcp_should_expand_sndbuf(sk)) {
		int sndmem = SKB_TRUESIZE(max_t(u32,
						tp->rx_opt.mss_clamp,
						tp->mss_cache) +
					  MAX_TCP_HEADER);
		int demanded = max_t(unsigned int, tp->snd_cwnd,
				     tp->reordering + 1);
		sndmem *= 2 * demanded;
		if (sndmem > sk->sk_sndbuf)
			sk->sk_sndbuf = min(sndmem, sysctl_tcp_wmem[2]);
		tp->snd_cwnd_stamp = tcp_time_stamp;
	}

	sk->sk_write_space(sk);
}