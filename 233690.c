static int irda_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;

	IRDA_DEBUG(2, "%s()\n", __func__);

	if ((sk->sk_type != SOCK_STREAM) && (sk->sk_type != SOCK_SEQPACKET) &&
	    (sk->sk_type != SOCK_DGRAM))
		return -EOPNOTSUPP;

	if (sk->sk_state != TCP_LISTEN) {
		sk->sk_max_ack_backlog = backlog;
		sk->sk_state           = TCP_LISTEN;

		return 0;
	}

	return -EOPNOTSUPP;
}