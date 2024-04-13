static inline int tcp_inq(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	int answ;

	if ((1 << sk->sk_state) & (TCPF_SYN_SENT | TCPF_SYN_RECV)) {
		answ = 0;
	} else if (sock_flag(sk, SOCK_URGINLINE) ||
		   !tp->urg_data ||
		   before(tp->urg_seq, tp->copied_seq) ||
		   !before(tp->urg_seq, tp->rcv_nxt)) {

		answ = tp->rcv_nxt - tp->copied_seq;

		/* Subtract 1, if FIN was received */
		if (answ && sock_flag(sk, SOCK_DONE))
			answ--;
	} else {
		answ = tp->urg_seq - tp->copied_seq;
	}

	return answ;
}