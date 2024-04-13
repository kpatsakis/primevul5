static inline void tcp_slow_start_after_idle_check(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	s32 delta;

	if (!sysctl_tcp_slow_start_after_idle || tp->packets_out)
		return;
	delta = tcp_time_stamp - tp->lsndtime;
	if (delta > inet_csk(sk)->icsk_rto)
		tcp_cwnd_restart(sk, delta);
}