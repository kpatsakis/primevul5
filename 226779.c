static void sctp_v6_destroy_sock(struct sock *sk)
{
	sctp_destroy_sock(sk);
	inet6_destroy_sock(sk);
}