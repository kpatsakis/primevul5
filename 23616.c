sctp_disposition_t sctp_sf_eat_auth(const struct sctp_endpoint *ep,
				    const struct sctp_association *asoc,
				    const sctp_subtype_t type,
				    void *arg,
				    sctp_cmd_seq_t *commands)
{
	struct sctp_authhdr *auth_hdr;
	struct sctp_chunk *chunk = arg;
	struct sctp_chunk *err_chunk;
	sctp_ierror_t error;

	/* Make sure that the peer has AUTH capable */
	if (!asoc->peer.auth_capable)
		return sctp_sf_unk_chunk(ep, asoc, type, arg, commands);

	if (!sctp_vtag_verify(chunk, asoc)) {
		sctp_add_cmd_sf(commands, SCTP_CMD_REPORT_BAD_TAG,
				SCTP_NULL());
		return sctp_sf_pdiscard(ep, asoc, type, arg, commands);
	}

	/* Make sure that the AUTH chunk has valid length.  */
	if (!sctp_chunk_length_valid(chunk, sizeof(struct sctp_auth_chunk)))
		return sctp_sf_violation_chunklen(ep, asoc, type, arg,
						  commands);

	auth_hdr = (struct sctp_authhdr *)chunk->skb->data;
	error = sctp_sf_authenticate(ep, asoc, type, chunk);
	switch (error) {
		case SCTP_IERROR_AUTH_BAD_HMAC:
			/* Generate the ERROR chunk and discard the rest
			 * of the packet
			 */
			err_chunk = sctp_make_op_error(asoc, chunk,
							SCTP_ERROR_UNSUP_HMAC,
							&auth_hdr->hmac_id,
							sizeof(__u16));
			if (err_chunk) {
				sctp_add_cmd_sf(commands, SCTP_CMD_REPLY,
						SCTP_CHUNK(err_chunk));
			}
			/* Fall Through */
		case SCTP_IERROR_AUTH_BAD_KEYID:
		case SCTP_IERROR_BAD_SIG:
			return sctp_sf_pdiscard(ep, asoc, type, arg, commands);
			break;
		case SCTP_IERROR_PROTO_VIOLATION:
			return sctp_sf_violation_chunklen(ep, asoc, type, arg,
							  commands);
			break;
		case SCTP_IERROR_NOMEM:
			return SCTP_DISPOSITION_NOMEM;
		default:
			break;
	}

	if (asoc->active_key_id != ntohs(auth_hdr->shkey_id)) {
		struct sctp_ulpevent *ev;

		ev = sctp_ulpevent_make_authkey(asoc, ntohs(auth_hdr->shkey_id),
				    SCTP_AUTH_NEWKEY, GFP_ATOMIC);

		if (!ev)
			return -ENOMEM;

		sctp_add_cmd_sf(commands, SCTP_CMD_EVENT_ULP,
				SCTP_ULPEVENT(ev));
	}

	return SCTP_DISPOSITION_CONSUME;
}