unsigned int xdr_read_pages(struct xdr_stream *xdr, unsigned int len)
{
	unsigned int nwords = XDR_QUADLEN(len);
	unsigned int base, end, pglen;

	pglen = xdr_align_pages(xdr, nwords << 2);
	if (pglen == 0)
		return 0;

	base = (nwords << 2) - pglen;
	end = xdr_stream_remaining(xdr) - pglen;

	xdr_set_tail_base(xdr, base, end);
	return len <= pglen ? len : pglen;
}