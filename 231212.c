static bool vhost_sock_xdp(struct socket *sock)
{
	return sock_flag(sock->sk, SOCK_XDP);
}