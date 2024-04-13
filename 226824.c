static int sctp_setsockopt_initmsg(struct sock *sk, struct sctp_initmsg *sinit,
				   unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);

	if (optlen != sizeof(struct sctp_initmsg))
		return -EINVAL;

	if (sinit->sinit_num_ostreams)
		sp->initmsg.sinit_num_ostreams = sinit->sinit_num_ostreams;
	if (sinit->sinit_max_instreams)
		sp->initmsg.sinit_max_instreams = sinit->sinit_max_instreams;
	if (sinit->sinit_max_attempts)
		sp->initmsg.sinit_max_attempts = sinit->sinit_max_attempts;
	if (sinit->sinit_max_init_timeo)
		sp->initmsg.sinit_max_init_timeo = sinit->sinit_max_init_timeo;

	return 0;
}