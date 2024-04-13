static bool sctp_writeable(struct sock *sk)
{
	return sk->sk_sndbuf > sk->sk_wmem_queued;
}