struct sctp_chunk *sctp_make_op_error(const struct sctp_association *asoc,
				 const struct sctp_chunk *chunk,
				 __be16 cause_code, const void *payload,
				 size_t paylen)
{
	struct sctp_chunk *retval;

	retval = sctp_make_op_error_space(asoc, chunk, paylen);
	if (!retval)
		goto nodata;

	sctp_init_cause(retval, cause_code, paylen);
	sctp_addto_chunk(retval, paylen, payload);

nodata:
	return retval;
}