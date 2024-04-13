static int sctp_setsockopt_reset_assoc(struct sock *sk, sctp_assoc_t *associd,
				       unsigned int optlen)
{
	struct sctp_association *asoc;

	if (optlen != sizeof(*associd))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, *associd);
	if (!asoc)
		return -EINVAL;

	return sctp_send_reset_assoc(asoc);
}