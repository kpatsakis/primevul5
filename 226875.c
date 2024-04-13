static int sctp_setsockopt_add_streams(struct sock *sk,
				       struct sctp_add_streams *params,
				       unsigned int optlen)
{
	struct sctp_association *asoc;

	if (optlen != sizeof(*params))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params->sas_assoc_id);
	if (!asoc)
		return -EINVAL;

	return sctp_send_add_streams(asoc, params);
}