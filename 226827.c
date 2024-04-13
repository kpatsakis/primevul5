int sctp_inet_connect(struct socket *sock, struct sockaddr *uaddr,
		      int addr_len, int flags)
{
	if (addr_len < sizeof(uaddr->sa_family))
		return -EINVAL;

	if (uaddr->sa_family == AF_UNSPEC)
		return -EOPNOTSUPP;

	return sctp_connect(sock->sk, uaddr, addr_len, flags);
}