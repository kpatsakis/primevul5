static int sctp_setsockopt_auth_chunk(struct sock *sk,
				      struct sctp_authchunk *val,
				      unsigned int optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;

	if (!ep->auth_enable)
		return -EACCES;

	if (optlen != sizeof(struct sctp_authchunk))
		return -EINVAL;

	switch (val->sauth_chunk) {
	case SCTP_CID_INIT:
	case SCTP_CID_INIT_ACK:
	case SCTP_CID_SHUTDOWN_COMPLETE:
	case SCTP_CID_AUTH:
		return -EINVAL;
	}

	/* add this chunk id to the endpoint */
	return sctp_auth_ep_add_chunkid(ep, val->sauth_chunk);
}