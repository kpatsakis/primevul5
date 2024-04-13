static int sctp_connect_new_asoc(struct sctp_endpoint *ep,
				 const union sctp_addr *daddr,
				 const struct sctp_initmsg *init,
				 struct sctp_transport **tp)
{
	struct sctp_association *asoc;
	struct sock *sk = ep->base.sk;
	struct net *net = sock_net(sk);
	enum sctp_scope scope;
	int err;

	if (sctp_endpoint_is_peeled_off(ep, daddr))
		return -EADDRNOTAVAIL;

	if (!ep->base.bind_addr.port) {
		if (sctp_autobind(sk))
			return -EAGAIN;
	} else {
		if (inet_port_requires_bind_service(net, ep->base.bind_addr.port) &&
		    !ns_capable(net->user_ns, CAP_NET_BIND_SERVICE))
			return -EACCES;
	}

	scope = sctp_scope(daddr);
	asoc = sctp_association_new(ep, sk, scope, GFP_KERNEL);
	if (!asoc)
		return -ENOMEM;

	err = sctp_assoc_set_bind_addr_from_ep(asoc, scope, GFP_KERNEL);
	if (err < 0)
		goto free;

	*tp = sctp_assoc_add_peer(asoc, daddr, GFP_KERNEL, SCTP_UNKNOWN);
	if (!*tp) {
		err = -ENOMEM;
		goto free;
	}

	if (!init)
		return 0;

	if (init->sinit_num_ostreams) {
		__u16 outcnt = init->sinit_num_ostreams;

		asoc->c.sinit_num_ostreams = outcnt;
		/* outcnt has been changed, need to re-init stream */
		err = sctp_stream_init(&asoc->stream, outcnt, 0, GFP_KERNEL);
		if (err)
			goto free;
	}

	if (init->sinit_max_instreams)
		asoc->c.sinit_max_instreams = init->sinit_max_instreams;

	if (init->sinit_max_attempts)
		asoc->max_init_attempts = init->sinit_max_attempts;

	if (init->sinit_max_init_timeo)
		asoc->max_init_timeo =
			msecs_to_jiffies(init->sinit_max_init_timeo);

	return 0;
free:
	sctp_association_free(asoc);
	return err;
}