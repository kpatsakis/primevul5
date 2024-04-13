static inline void tcp_listendrop(const struct sock *sk)
{
	atomic_inc(&((struct sock *)sk)->sk_drops);
	__NET_INC_STATS(sock_net(sk), LINUX_MIB_LISTENDROPS);
}