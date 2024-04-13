static int sctp_setsockopt_adaptation_layer(struct sock *sk,
					    struct sctp_setadaptation *adapt,
					    unsigned int optlen)
{
	if (optlen != sizeof(struct sctp_setadaptation))
		return -EINVAL;

	sctp_sk(sk)->adaptation_ind = adapt->ssb_adaptation_ind;

	return 0;
}