static int sctp_asconf_param_success(struct sctp_association *asoc,
				     sctp_addip_param_t *asconf_param)
{
	struct sctp_af *af;
	union sctp_addr	addr;
	struct sctp_bind_addr *bp = &asoc->base.bind_addr;
	union sctp_addr_param *addr_param;
	struct sctp_transport *transport;
	struct sctp_sockaddr_entry *saddr;
	int retval = 0;

	addr_param = (union sctp_addr_param *)
			((void *)asconf_param + sizeof(sctp_addip_param_t));

	/* We have checked the packet before, so we do not check again.	*/
	af = sctp_get_af_specific(param_type2af(addr_param->v4.param_hdr.type));
	af->from_addr_param(&addr, addr_param, htons(bp->port), 0);

	switch (asconf_param->param_hdr.type) {
	case SCTP_PARAM_ADD_IP:
		/* This is always done in BH context with a socket lock
		 * held, so the list can not change.
		 */
		local_bh_disable();
		list_for_each_entry(saddr, &bp->address_list, list) {
			if (sctp_cmp_addr_exact(&saddr->a, &addr))
				saddr->state = SCTP_ADDR_SRC;
		}
		local_bh_enable();
		break;
	case SCTP_PARAM_DEL_IP:
		local_bh_disable();
		retval = sctp_del_bind_addr(bp, &addr);
		local_bh_enable();
		list_for_each_entry(transport, &asoc->peer.transport_addr_list,
				transports) {
			dst_release(transport->dst);
			sctp_transport_route(transport, NULL,
					     sctp_sk(asoc->base.sk));
		}
		break;
	default:
		break;
	}

	return retval;
}