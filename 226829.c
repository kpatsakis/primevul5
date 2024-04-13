static int sctp_sendmsg_new_asoc(struct sock *sk, __u16 sflags,
				 struct sctp_cmsgs *cmsgs,
				 union sctp_addr *daddr,
				 struct sctp_transport **tp)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_association *asoc;
	struct cmsghdr *cmsg;
	__be32 flowinfo = 0;
	struct sctp_af *af;
	int err;

	*tp = NULL;

	if (sflags & (SCTP_EOF | SCTP_ABORT))
		return -EINVAL;

	if (sctp_style(sk, TCP) && (sctp_sstate(sk, ESTABLISHED) ||
				    sctp_sstate(sk, CLOSING)))
		return -EADDRNOTAVAIL;

	/* Label connection socket for first association 1-to-many
	 * style for client sequence socket()->sendmsg(). This
	 * needs to be done before sctp_assoc_add_peer() as that will
	 * set up the initial packet that needs to account for any
	 * security ip options (CIPSO/CALIPSO) added to the packet.
	 */
	af = sctp_get_af_specific(daddr->sa.sa_family);
	if (!af)
		return -EINVAL;
	err = security_sctp_bind_connect(sk, SCTP_SENDMSG_CONNECT,
					 (struct sockaddr *)daddr,
					 af->sockaddr_len);
	if (err < 0)
		return err;

	err = sctp_connect_new_asoc(ep, daddr, cmsgs->init, tp);
	if (err)
		return err;
	asoc = (*tp)->asoc;

	if (!cmsgs->addrs_msg)
		return 0;

	if (daddr->sa.sa_family == AF_INET6)
		flowinfo = daddr->v6.sin6_flowinfo;

	/* sendv addr list parse */
	for_each_cmsghdr(cmsg, cmsgs->addrs_msg) {
		union sctp_addr _daddr;
		int dlen;

		if (cmsg->cmsg_level != IPPROTO_SCTP ||
		    (cmsg->cmsg_type != SCTP_DSTADDRV4 &&
		     cmsg->cmsg_type != SCTP_DSTADDRV6))
			continue;

		daddr = &_daddr;
		memset(daddr, 0, sizeof(*daddr));
		dlen = cmsg->cmsg_len - sizeof(struct cmsghdr);
		if (cmsg->cmsg_type == SCTP_DSTADDRV4) {
			if (dlen < sizeof(struct in_addr)) {
				err = -EINVAL;
				goto free;
			}

			dlen = sizeof(struct in_addr);
			daddr->v4.sin_family = AF_INET;
			daddr->v4.sin_port = htons(asoc->peer.port);
			memcpy(&daddr->v4.sin_addr, CMSG_DATA(cmsg), dlen);
		} else {
			if (dlen < sizeof(struct in6_addr)) {
				err = -EINVAL;
				goto free;
			}

			dlen = sizeof(struct in6_addr);
			daddr->v6.sin6_flowinfo = flowinfo;
			daddr->v6.sin6_family = AF_INET6;
			daddr->v6.sin6_port = htons(asoc->peer.port);
			memcpy(&daddr->v6.sin6_addr, CMSG_DATA(cmsg), dlen);
		}

		err = sctp_connect_add_peer(asoc, daddr, sizeof(*daddr));
		if (err)
			goto free;
	}

	return 0;

free:
	sctp_association_free(asoc);
	return err;
}