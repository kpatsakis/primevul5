static int sctp_setsockopt_connectx_old(struct sock *sk,
					struct sockaddr *kaddrs,
					int addrs_size)
{
	return __sctp_setsockopt_connectx(sk, kaddrs, addrs_size, NULL);
}