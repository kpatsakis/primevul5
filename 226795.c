static int sctp_setsockopt_asconf_supported(struct sock *sk,
					    struct sctp_assoc_value *params,
					    unsigned int optlen)
{
	struct sctp_association *asoc;
	struct sctp_endpoint *ep;
	int retval = -EINVAL;

	if (optlen != sizeof(*params))
		goto out;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	ep = sctp_sk(sk)->ep;
	ep->asconf_enable = !!params->assoc_value;

	if (ep->asconf_enable && ep->auth_enable) {
		sctp_auth_ep_add_chunkid(ep, SCTP_CID_ASCONF);
		sctp_auth_ep_add_chunkid(ep, SCTP_CID_ASCONF_ACK);
	}

	retval = 0;

out:
	return retval;
}