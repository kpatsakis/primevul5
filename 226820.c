static int sctp_setsockopt_connectx(struct sock *sk,
				    struct sockaddr *kaddrs,
				    int addrs_size)
{
	sctp_assoc_t assoc_id = 0;
	int err = 0;

	err = __sctp_setsockopt_connectx(sk, kaddrs, addrs_size, &assoc_id);

	if (err)
		return err;
	else
		return assoc_id;
}