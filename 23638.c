int sctp_process_asconf_ack(struct sctp_association *asoc,
			    struct sctp_chunk *asconf_ack)
{
	struct sctp_chunk	*asconf = asoc->addip_last_asconf;
	union sctp_addr_param	*addr_param;
	sctp_addip_param_t	*asconf_param;
	int	length = 0;
	int	asconf_len = asconf->skb->len;
	int	all_param_pass = 0;
	int	no_err = 1;
	int	retval = 0;
	__be16	err_code = SCTP_ERROR_NO_ERROR;

	/* Skip the chunkhdr and addiphdr from the last asconf sent and store
	 * a pointer to address parameter.
	 */
	length = sizeof(sctp_addip_chunk_t);
	addr_param = (union sctp_addr_param *)(asconf->skb->data + length);
	asconf_len -= length;

	/* Skip the address parameter in the last asconf sent and store a
	 * pointer to the first asconf parameter.
	 */
	length = ntohs(addr_param->v4.param_hdr.length);
	asconf_param = (sctp_addip_param_t *)((void *)addr_param + length);
	asconf_len -= length;

	/* ADDIP 4.1
	 * A8) If there is no response(s) to specific TLV parameter(s), and no
	 * failures are indicated, then all request(s) are considered
	 * successful.
	 */
	if (asconf_ack->skb->len == sizeof(sctp_addiphdr_t))
		all_param_pass = 1;

	/* Process the TLVs contained in the last sent ASCONF chunk. */
	while (asconf_len > 0) {
		if (all_param_pass)
			err_code = SCTP_ERROR_NO_ERROR;
		else {
			err_code = sctp_get_asconf_response(asconf_ack,
							    asconf_param,
							    no_err);
			if (no_err && (SCTP_ERROR_NO_ERROR != err_code))
				no_err = 0;
		}

		switch (err_code) {
		case SCTP_ERROR_NO_ERROR:
			retval = sctp_asconf_param_success(asoc, asconf_param);
			break;

		case SCTP_ERROR_RSRC_LOW:
			retval = 1;
			break;

		case SCTP_ERROR_INV_PARAM:
			/* Disable sending this type of asconf parameter in
			 * future.
			 */
			asoc->peer.addip_disabled_mask |=
				asconf_param->param_hdr.type;
			break;

		case SCTP_ERROR_REQ_REFUSED:
		case SCTP_ERROR_DEL_LAST_IP:
		case SCTP_ERROR_DEL_SRC_IP:
		default:
			 break;
		}

		/* Skip the processed asconf parameter and move to the next
		 * one.
		 */
		length = ntohs(asconf_param->param_hdr.length);
		asconf_param = (sctp_addip_param_t *)((void *)asconf_param +
						      length);
		asconf_len -= length;
	}

	/* Free the cached last sent asconf chunk. */
	list_del_init(&asconf->transmitted_list);
	sctp_chunk_free(asconf);
	asoc->addip_last_asconf = NULL;

	/* Send the next asconf chunk from the addip chunk queue. */
	if (!list_empty(&asoc->addip_chunk_list)) {
		struct list_head *entry = asoc->addip_chunk_list.next;
		asconf = list_entry(entry, struct sctp_chunk, list);

		list_del_init(entry);

		/* Hold the chunk until an ASCONF_ACK is received. */
		sctp_chunk_hold(asconf);
		if (sctp_primitive_ASCONF(asoc, asconf))
			sctp_chunk_free(asconf);
		else
			asoc->addip_last_asconf = asconf;
	}

	return retval;
}