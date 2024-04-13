static int rawsock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;

	pr_debug("sock=%p sk=%p\n", sock, sk);

	if (!sk)
		return 0;

	if (sock->type == SOCK_RAW)
		nfc_sock_unlink(&raw_sk_list, sk);

	sock_orphan(sk);
	sock_put(sk);

	return 0;
}