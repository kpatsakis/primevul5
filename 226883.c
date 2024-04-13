static int sctp_setsockopt_primary_addr(struct sock *sk, struct sctp_prim *prim,
					unsigned int optlen)
{
	struct sctp_transport *trans;
	struct sctp_af *af;
	int err;

	if (optlen != sizeof(struct sctp_prim))
		return -EINVAL;

	/* Allow security module to validate address but need address len. */
	af = sctp_get_af_specific(prim->ssp_addr.ss_family);
	if (!af)
		return -EINVAL;

	err = security_sctp_bind_connect(sk, SCTP_PRIMARY_ADDR,
					 (struct sockaddr *)&prim->ssp_addr,
					 af->sockaddr_len);
	if (err)
		return err;

	trans = sctp_addr_id2transport(sk, &prim->ssp_addr, prim->ssp_assoc_id);
	if (!trans)
		return -EINVAL;

	sctp_assoc_set_primary(trans->asoc, trans);

	return 0;
}