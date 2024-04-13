static int sctp_setsockopt_hmac_ident(struct sock *sk,
				      struct sctp_hmacalgo *hmacs,
				      unsigned int optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	u32 idents;

	if (!ep->auth_enable)
		return -EACCES;

	if (optlen < sizeof(struct sctp_hmacalgo))
		return -EINVAL;
	optlen = min_t(unsigned int, optlen, sizeof(struct sctp_hmacalgo) +
					     SCTP_AUTH_NUM_HMACS * sizeof(u16));

	idents = hmacs->shmac_num_idents;
	if (idents == 0 || idents > SCTP_AUTH_NUM_HMACS ||
	    (idents * sizeof(u16)) > (optlen - sizeof(struct sctp_hmacalgo)))
		return -EINVAL;

	return sctp_auth_ep_set_hmacs(ep, hmacs);
}