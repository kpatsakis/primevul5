static int sctp_setsockopt_partial_delivery_point(struct sock *sk, u32 *val,
						  unsigned int optlen)
{
	if (optlen != sizeof(u32))
		return -EINVAL;

	/* Note: We double the receive buffer from what the user sets
	 * it to be, also initial rwnd is based on rcvbuf/2.
	 */
	if (*val > (sk->sk_rcvbuf >> 1))
		return -EINVAL;

	sctp_sk(sk)->pd_point = *val;

	return 0; /* is this the right error code? */
}