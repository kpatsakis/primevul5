static int sctp_setsockopt_reset_streams(struct sock *sk,
					 struct sctp_reset_streams *params,
					 unsigned int optlen)
{
	struct sctp_association *asoc;

	if (optlen < sizeof(*params))
		return -EINVAL;
	/* srs_number_streams is u16, so optlen can't be bigger than this. */
	optlen = min_t(unsigned int, optlen, USHRT_MAX +
					     sizeof(__u16) * sizeof(*params));

	if (params->srs_number_streams * sizeof(__u16) >
	    optlen - sizeof(*params))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params->srs_assoc_id);
	if (!asoc)
		return -EINVAL;

	return sctp_send_reset_streams(asoc, params);
}