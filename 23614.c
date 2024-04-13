struct sctp_chunk *sctp_make_fwdtsn(const struct sctp_association *asoc,
				    __u32 new_cum_tsn, size_t nstreams,
				    struct sctp_fwdtsn_skip *skiplist)
{
	struct sctp_chunk *retval = NULL;
	struct sctp_fwdtsn_chunk *ftsn_chunk;
	struct sctp_fwdtsn_hdr ftsn_hdr;
	struct sctp_fwdtsn_skip skip;
	size_t hint;
	int i;

	hint = (nstreams + 1) * sizeof(__u32);

	retval = sctp_make_chunk(asoc, SCTP_CID_FWD_TSN, 0, hint);

	if (!retval)
		return NULL;

	ftsn_chunk = (struct sctp_fwdtsn_chunk *)retval->subh.fwdtsn_hdr;

	ftsn_hdr.new_cum_tsn = htonl(new_cum_tsn);
	retval->subh.fwdtsn_hdr =
		sctp_addto_chunk(retval, sizeof(ftsn_hdr), &ftsn_hdr);

	for (i = 0; i < nstreams; i++) {
		skip.stream = skiplist[i].stream;
		skip.ssn = skiplist[i].ssn;
		sctp_addto_chunk(retval, sizeof(skip), &skip);
	}

	return retval;
}