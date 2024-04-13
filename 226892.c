static int sctp_connect_add_peer(struct sctp_association *asoc,
				 union sctp_addr *daddr, int addr_len)
{
	struct sctp_endpoint *ep = asoc->ep;
	struct sctp_association *old;
	struct sctp_transport *t;
	int err;

	err = sctp_verify_addr(ep->base.sk, daddr, addr_len);
	if (err)
		return err;

	old = sctp_endpoint_lookup_assoc(ep, daddr, &t);
	if (old && old != asoc)
		return old->state >= SCTP_STATE_ESTABLISHED ? -EISCONN
							    : -EALREADY;

	if (sctp_endpoint_is_peeled_off(ep, daddr))
		return -EADDRNOTAVAIL;

	t = sctp_assoc_add_peer(asoc, daddr, GFP_KERNEL, SCTP_UNKNOWN);
	if (!t)
		return -ENOMEM;

	return 0;
}