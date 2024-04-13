static void x25_destroy_timer(struct timer_list *t)
{
	struct sock *sk = from_timer(sk, t, sk_timer);

	x25_destroy_socket_from_timer(sk);
}