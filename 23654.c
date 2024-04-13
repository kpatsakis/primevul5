struct sctp_chunk *sctp_make_heartbeat(const struct sctp_association *asoc,
				  const struct sctp_transport *transport,
				  const void *payload, const size_t paylen)
{
	struct sctp_chunk *retval = sctp_make_chunk(asoc, SCTP_CID_HEARTBEAT,
						    0, paylen);

	if (!retval)
		goto nodata;

	/* Cast away the 'const', as this is just telling the chunk
	 * what transport it belongs to.
	 */
	retval->transport = (struct sctp_transport *) transport;
	retval->subh.hbs_hdr = sctp_addto_chunk(retval, paylen, payload);

nodata:
	return retval;
}