static int sctp_setsockopt_encap_port(struct sock *sk,
				      struct sctp_udpencaps *encap,
				      unsigned int optlen)
{
	struct sctp_association *asoc;
	struct sctp_transport *t;
	__be16 encap_port;

	if (optlen != sizeof(*encap))
		return -EINVAL;

	/* If an address other than INADDR_ANY is specified, and
	 * no transport is found, then the request is invalid.
	 */
	encap_port = (__force __be16)encap->sue_port;
	if (!sctp_is_any(sk, (union sctp_addr *)&encap->sue_address)) {
		t = sctp_addr_id2transport(sk, &encap->sue_address,
					   encap->sue_assoc_id);
		if (!t)
			return -EINVAL;

		t->encap_port = encap_port;
		return 0;
	}

	/* Get association, if assoc_id != SCTP_FUTURE_ASSOC and the
	 * socket is a one to many style socket, and an association
	 * was not found, then the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, encap->sue_assoc_id);
	if (!asoc && encap->sue_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	/* If changes are for association, also apply encap_port to
	 * each transport.
	 */
	if (asoc) {
		list_for_each_entry(t, &asoc->peer.transport_addr_list,
				    transports)
			t->encap_port = encap_port;

		return 0;
	}

	sctp_sk(sk)->encap_port = encap_port;
	return 0;
}