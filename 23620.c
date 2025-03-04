sctp_disposition_t sctp_sf_do_asconf(const struct sctp_endpoint *ep,
				     const struct sctp_association *asoc,
				     const sctp_subtype_t type, void *arg,
				     sctp_cmd_seq_t *commands)
{
	struct sctp_chunk	*chunk = arg;
	struct sctp_chunk	*asconf_ack = NULL;
	struct sctp_paramhdr	*err_param = NULL;
	sctp_addiphdr_t		*hdr;
	union sctp_addr_param	*addr_param;
	__u32			serial;
	int			length;

	if (!sctp_vtag_verify(chunk, asoc)) {
		sctp_add_cmd_sf(commands, SCTP_CMD_REPORT_BAD_TAG,
				SCTP_NULL());
		return sctp_sf_pdiscard(ep, asoc, type, arg, commands);
	}

	/* ADD-IP: Section 4.1.1
	 * This chunk MUST be sent in an authenticated way by using
	 * the mechanism defined in [I-D.ietf-tsvwg-sctp-auth]. If this chunk
	 * is received unauthenticated it MUST be silently discarded as
	 * described in [I-D.ietf-tsvwg-sctp-auth].
	 */
	if (!sctp_addip_noauth && !chunk->auth)
		return sctp_sf_discard_chunk(ep, asoc, type, arg, commands);

	/* Make sure that the ASCONF ADDIP chunk has a valid length.  */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_addip_chunk_t)))
		return sctp_sf_violation_chunklen(ep, asoc, type, arg,
						  commands);

	hdr = (sctp_addiphdr_t *)chunk->skb->data;
	serial = ntohl(hdr->serial);

	addr_param = (union sctp_addr_param *)hdr->params;
	length = ntohs(addr_param->p.length);
	if (length < sizeof(sctp_paramhdr_t))
		return sctp_sf_violation_paramlen(ep, asoc, type, arg,
			   (void *)addr_param, commands);

	/* Verify the ASCONF chunk before processing it. */
	if (!sctp_verify_asconf(asoc,
			    (sctp_paramhdr_t *)((void *)addr_param + length),
			    (void *)chunk->chunk_end,
			    &err_param))
		return sctp_sf_violation_paramlen(ep, asoc, type, arg,
						  (void *)err_param, commands);

	/* ADDIP 5.2 E1) Compare the value of the serial number to the value
	 * the endpoint stored in a new association variable
	 * 'Peer-Serial-Number'.
	 */
	if (serial == asoc->peer.addip_serial + 1) {
		/* If this is the first instance of ASCONF in the packet,
		 * we can clean our old ASCONF-ACKs.
		 */
		if (!chunk->has_asconf)
			sctp_assoc_clean_asconf_ack_cache(asoc);

		/* ADDIP 5.2 E4) When the Sequence Number matches the next one
		 * expected, process the ASCONF as described below and after
		 * processing the ASCONF Chunk, append an ASCONF-ACK Chunk to
		 * the response packet and cache a copy of it (in the event it
		 * later needs to be retransmitted).
		 *
		 * Essentially, do V1-V5.
		 */
		asconf_ack = sctp_process_asconf((struct sctp_association *)
						 asoc, chunk);
		if (!asconf_ack)
			return SCTP_DISPOSITION_NOMEM;
	} else if (serial < asoc->peer.addip_serial + 1) {
		/* ADDIP 5.2 E2)
		 * If the value found in the Sequence Number is less than the
		 * ('Peer- Sequence-Number' + 1), simply skip to the next
		 * ASCONF, and include in the outbound response packet
		 * any previously cached ASCONF-ACK response that was
		 * sent and saved that matches the Sequence Number of the
		 * ASCONF.  Note: It is possible that no cached ASCONF-ACK
		 * Chunk exists.  This will occur when an older ASCONF
		 * arrives out of order.  In such a case, the receiver
		 * should skip the ASCONF Chunk and not include ASCONF-ACK
		 * Chunk for that chunk.
		 */
		asconf_ack = sctp_assoc_lookup_asconf_ack(asoc, hdr->serial);
		if (!asconf_ack)
			return SCTP_DISPOSITION_DISCARD;
	} else {
		/* ADDIP 5.2 E5) Otherwise, the ASCONF Chunk is discarded since
		 * it must be either a stale packet or from an attacker.
		 */
		return SCTP_DISPOSITION_DISCARD;
	}

	/* ADDIP 5.2 E6)  The destination address of the SCTP packet
	 * containing the ASCONF-ACK Chunks MUST be the source address of
	 * the SCTP packet that held the ASCONF Chunks.
	 *
	 * To do this properly, we'll set the destination address of the chunk
	 * and at the transmit time, will try look up the transport to use.
	 * Since ASCONFs may be bundled, the correct transport may not be
	 * created untill we process the entire packet, thus this workaround.
	 */
	asconf_ack->dest = chunk->source;
	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(asconf_ack));

	return SCTP_DISPOSITION_CONSUME;
}