static void tcp_try_to_open(struct sock *sk, int flag)
{
	struct tcp_sock *tp = tcp_sk(sk);

	tcp_verify_left_out(tp);

	if (!tp->frto_counter && !tcp_any_retrans_done(sk))
		tp->retrans_stamp = 0;

	if (flag & FLAG_ECE)
		tcp_enter_cwr(sk, 1);

	if (inet_csk(sk)->icsk_ca_state != TCP_CA_CWR) {
		tcp_try_keep_open(sk);
		if (inet_csk(sk)->icsk_ca_state != TCP_CA_Open)
			tcp_moderate_cwnd(tp);
	} else {
		tcp_cwnd_down(sk, flag);
	}
}