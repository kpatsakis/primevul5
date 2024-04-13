static int sctp_setsockopt_peer_primary_addr(struct sock *sk,
					     struct sctp_setpeerprim *prim,
					     unsigned int optlen)
{
	struct sctp_sock	*sp;
	struct sctp_association	*asoc = NULL;
	struct sctp_chunk	*chunk;
	struct sctp_af		*af;
	int 			err;

	sp = sctp_sk(sk);

	if (!sp->ep->asconf_enable)
		return -EPERM;

	if (optlen != sizeof(struct sctp_setpeerprim))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, prim->sspp_assoc_id);
	if (!asoc)
		return -EINVAL;

	if (!asoc->peer.asconf_capable)
		return -EPERM;

	if (asoc->peer.addip_disabled_mask & SCTP_PARAM_SET_PRIMARY)
		return -EPERM;

	if (!sctp_state(asoc, ESTABLISHED))
		return -ENOTCONN;

	af = sctp_get_af_specific(prim->sspp_addr.ss_family);
	if (!af)
		return -EINVAL;

	if (!af->addr_valid((union sctp_addr *)&prim->sspp_addr, sp, NULL))
		return -EADDRNOTAVAIL;

	if (!sctp_assoc_lookup_laddr(asoc, (union sctp_addr *)&prim->sspp_addr))
		return -EADDRNOTAVAIL;

	/* Allow security module to validate address. */
	err = security_sctp_bind_connect(sk, SCTP_SET_PEER_PRIMARY_ADDR,
					 (struct sockaddr *)&prim->sspp_addr,
					 af->sockaddr_len);
	if (err)
		return err;

	/* Create an ASCONF chunk with SET_PRIMARY parameter	*/
	chunk = sctp_make_asconf_set_prim(asoc,
					  (union sctp_addr *)&prim->sspp_addr);
	if (!chunk)
		return -ENOMEM;

	err = sctp_send_asconf(asoc, chunk);

	pr_debug("%s: we set peer primary addr primitively\n", __func__);

	return err;
}