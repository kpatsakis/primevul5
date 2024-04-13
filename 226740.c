static int __sctp_connect(struct sock *sk, struct sockaddr *kaddrs,
			  int addrs_size, int flags, sctp_assoc_t *assoc_id)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_endpoint *ep = sp->ep;
	struct sctp_transport *transport;
	struct sctp_association *asoc;
	void *addr_buf = kaddrs;
	union sctp_addr *daddr;
	struct sctp_af *af;
	int walk_size, err;
	long timeo;

	if (sctp_sstate(sk, ESTABLISHED) || sctp_sstate(sk, CLOSING) ||
	    (sctp_style(sk, TCP) && sctp_sstate(sk, LISTENING)))
		return -EISCONN;

	daddr = addr_buf;
	af = sctp_get_af_specific(daddr->sa.sa_family);
	if (!af || af->sockaddr_len > addrs_size)
		return -EINVAL;

	err = sctp_verify_addr(sk, daddr, af->sockaddr_len);
	if (err)
		return err;

	asoc = sctp_endpoint_lookup_assoc(ep, daddr, &transport);
	if (asoc)
		return asoc->state >= SCTP_STATE_ESTABLISHED ? -EISCONN
							     : -EALREADY;

	err = sctp_connect_new_asoc(ep, daddr, NULL, &transport);
	if (err)
		return err;
	asoc = transport->asoc;

	addr_buf += af->sockaddr_len;
	walk_size = af->sockaddr_len;
	while (walk_size < addrs_size) {
		err = -EINVAL;
		if (walk_size + sizeof(sa_family_t) > addrs_size)
			goto out_free;

		daddr = addr_buf;
		af = sctp_get_af_specific(daddr->sa.sa_family);
		if (!af || af->sockaddr_len + walk_size > addrs_size)
			goto out_free;

		if (asoc->peer.port != ntohs(daddr->v4.sin_port))
			goto out_free;

		err = sctp_connect_add_peer(asoc, daddr, af->sockaddr_len);
		if (err)
			goto out_free;

		addr_buf  += af->sockaddr_len;
		walk_size += af->sockaddr_len;
	}

	/* In case the user of sctp_connectx() wants an association
	 * id back, assign one now.
	 */
	if (assoc_id) {
		err = sctp_assoc_set_id(asoc, GFP_KERNEL);
		if (err < 0)
			goto out_free;
	}

	err = sctp_primitive_ASSOCIATE(sock_net(sk), asoc, NULL);
	if (err < 0)
		goto out_free;

	/* Initialize sk's dport and daddr for getpeername() */
	inet_sk(sk)->inet_dport = htons(asoc->peer.port);
	sp->pf->to_sk_daddr(daddr, sk);
	sk->sk_err = 0;

	if (assoc_id)
		*assoc_id = asoc->assoc_id;

	timeo = sock_sndtimeo(sk, flags & O_NONBLOCK);
	return sctp_wait_for_connect(asoc, &timeo);

out_free:
	pr_debug("%s: took out_free path with asoc:%p kaddrs:%p err:%d\n",
		 __func__, asoc, kaddrs, err);
	sctp_association_free(asoc);
	return err;
}