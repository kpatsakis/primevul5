static inline bool inet_sk_transparent(const struct sock *sk)
{
	switch (sk->sk_state) {
	case TCP_TIME_WAIT:
		return inet_twsk(sk)->tw_transparent;
	case TCP_NEW_SYN_RECV:
		return inet_rsk(inet_reqsk(sk))->no_srccheck;
	}
	return inet_sk(sk)->transparent;
}