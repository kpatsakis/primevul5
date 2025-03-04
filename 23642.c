static int sctp_process_param(struct sctp_association *asoc,
			      union sctp_params param,
			      const union sctp_addr *peer_addr,
			      gfp_t gfp)
{
	union sctp_addr addr;
	int i;
	__u16 sat;
	int retval = 1;
	sctp_scope_t scope;
	time_t stale;
	struct sctp_af *af;
	union sctp_addr_param *addr_param;
	struct sctp_transport *t;

	/* We maintain all INIT parameters in network byte order all the
	 * time.  This allows us to not worry about whether the parameters
	 * came from a fresh INIT, and INIT ACK, or were stored in a cookie.
	 */
	switch (param.p->type) {
	case SCTP_PARAM_IPV6_ADDRESS:
		if (PF_INET6 != asoc->base.sk->sk_family)
			break;
		goto do_addr_param;

	case SCTP_PARAM_IPV4_ADDRESS:
		/* v4 addresses are not allowed on v6-only socket */
		if (ipv6_only_sock(asoc->base.sk))
			break;
do_addr_param:
		af = sctp_get_af_specific(param_type2af(param.p->type));
		af->from_addr_param(&addr, param.addr, htons(asoc->peer.port), 0);
		scope = sctp_scope(peer_addr);
		if (sctp_in_scope(&addr, scope))
			if (!sctp_assoc_add_peer(asoc, &addr, gfp, SCTP_UNCONFIRMED))
				return 0;
		break;

	case SCTP_PARAM_COOKIE_PRESERVATIVE:
		if (!sctp_cookie_preserve_enable)
			break;

		stale = ntohl(param.life->lifespan_increment);

		/* Suggested Cookie Life span increment's unit is msec,
		 * (1/1000sec).
		 */
		asoc->cookie_life.tv_sec += stale / 1000;
		asoc->cookie_life.tv_usec += (stale % 1000) * 1000;
		break;

	case SCTP_PARAM_HOST_NAME_ADDRESS:
		SCTP_DEBUG_PRINTK("unimplemented SCTP_HOST_NAME_ADDRESS\n");
		break;

	case SCTP_PARAM_SUPPORTED_ADDRESS_TYPES:
		/* Turn off the default values first so we'll know which
		 * ones are really set by the peer.
		 */
		asoc->peer.ipv4_address = 0;
		asoc->peer.ipv6_address = 0;

		/* Assume that peer supports the address family
		 * by which it sends a packet.
		 */
		if (peer_addr->sa.sa_family == AF_INET6)
			asoc->peer.ipv6_address = 1;
		else if (peer_addr->sa.sa_family == AF_INET)
			asoc->peer.ipv4_address = 1;

		/* Cycle through address types; avoid divide by 0. */
		sat = ntohs(param.p->length) - sizeof(sctp_paramhdr_t);
		if (sat)
			sat /= sizeof(__u16);

		for (i = 0; i < sat; ++i) {
			switch (param.sat->types[i]) {
			case SCTP_PARAM_IPV4_ADDRESS:
				asoc->peer.ipv4_address = 1;
				break;

			case SCTP_PARAM_IPV6_ADDRESS:
				if (PF_INET6 == asoc->base.sk->sk_family)
					asoc->peer.ipv6_address = 1;
				break;

			case SCTP_PARAM_HOST_NAME_ADDRESS:
				asoc->peer.hostname_address = 1;
				break;

			default: /* Just ignore anything else.  */
				break;
			}
		}
		break;

	case SCTP_PARAM_STATE_COOKIE:
		asoc->peer.cookie_len =
			ntohs(param.p->length) - sizeof(sctp_paramhdr_t);
		asoc->peer.cookie = param.cookie->body;
		break;

	case SCTP_PARAM_HEARTBEAT_INFO:
		/* Would be odd to receive, but it causes no problems. */
		break;

	case SCTP_PARAM_UNRECOGNIZED_PARAMETERS:
		/* Rejected during verify stage. */
		break;

	case SCTP_PARAM_ECN_CAPABLE:
		asoc->peer.ecn_capable = 1;
		break;

	case SCTP_PARAM_ADAPTATION_LAYER_IND:
		asoc->peer.adaptation_ind = param.aind->adaptation_ind;
		break;

	case SCTP_PARAM_SET_PRIMARY:
		if (!sctp_addip_enable)
			goto fall_through;

		addr_param = param.v + sizeof(sctp_addip_param_t);

		af = sctp_get_af_specific(param_type2af(param.p->type));
		af->from_addr_param(&addr, addr_param,
				    htons(asoc->peer.port), 0);

		/* if the address is invalid, we can't process it.
		 * XXX: see spec for what to do.
		 */
		if (!af->addr_valid(&addr, NULL, NULL))
			break;

		t = sctp_assoc_lookup_paddr(asoc, &addr);
		if (!t)
			break;

		sctp_assoc_set_primary(asoc, t);
		break;

	case SCTP_PARAM_SUPPORTED_EXT:
		sctp_process_ext_param(asoc, param);
		break;

	case SCTP_PARAM_FWD_TSN_SUPPORT:
		if (sctp_prsctp_enable) {
			asoc->peer.prsctp_capable = 1;
			break;
		}
		/* Fall Through */
		goto fall_through;

	case SCTP_PARAM_RANDOM:
		if (!sctp_auth_enable)
			goto fall_through;

		/* Save peer's random parameter */
		asoc->peer.peer_random = kmemdup(param.p,
					    ntohs(param.p->length), gfp);
		if (!asoc->peer.peer_random) {
			retval = 0;
			break;
		}
		break;

	case SCTP_PARAM_HMAC_ALGO:
		if (!sctp_auth_enable)
			goto fall_through;

		/* Save peer's HMAC list */
		asoc->peer.peer_hmacs = kmemdup(param.p,
					    ntohs(param.p->length), gfp);
		if (!asoc->peer.peer_hmacs) {
			retval = 0;
			break;
		}

		/* Set the default HMAC the peer requested*/
		sctp_auth_asoc_set_default_hmac(asoc, param.hmac_algo);
		break;

	case SCTP_PARAM_CHUNKS:
		if (!sctp_auth_enable)
			goto fall_through;

		asoc->peer.peer_chunks = kmemdup(param.p,
					    ntohs(param.p->length), gfp);
		if (!asoc->peer.peer_chunks)
			retval = 0;
		break;
fall_through:
	default:
		/* Any unrecognized parameters should have been caught
		 * and handled by sctp_verify_param() which should be
		 * called prior to this routine.  Simply log the error
		 * here.
		 */
		SCTP_DEBUG_PRINTK("Ignoring param: %d for association %p.\n",
				  ntohs(param.p->type), asoc);
		break;
	}

	return retval;
}