static int verify_attr_len(struct xdr_stream *xdr, unsigned int savep, uint32_t attrlen)
{
	unsigned int attrwords = XDR_QUADLEN(attrlen);
	unsigned int nwords = (xdr_stream_pos(xdr) - savep) >> 2;

	if (unlikely(attrwords != nwords)) {
		dprintk("%s: server returned incorrect attribute length: "
			"%u %c %u\n",
				__func__,
				attrwords << 2,
				(attrwords < nwords) ? '<' : '>',
				nwords << 2);
		return -EIO;
	}
	return 0;
}