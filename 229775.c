static int decode_pathconf(struct xdr_stream *xdr, struct nfs_pathconf *pathconf)
{
	unsigned int savep;
	uint32_t attrlen, bitmap[3] = {0};
	int status;

	if ((status = decode_op_hdr(xdr, OP_GETATTR)) != 0)
		goto xdr_error;
	if ((status = decode_attr_bitmap(xdr, bitmap)) != 0)
		goto xdr_error;
	if ((status = decode_attr_length(xdr, &attrlen, &savep)) != 0)
		goto xdr_error;

	if ((status = decode_attr_maxlink(xdr, bitmap, &pathconf->max_link)) != 0)
		goto xdr_error;
	if ((status = decode_attr_maxname(xdr, bitmap, &pathconf->max_namelen)) != 0)
		goto xdr_error;

	status = verify_attr_len(xdr, savep, attrlen);
xdr_error:
	dprintk("%s: xdr returned %d!\n", __func__, -status);
	return status;
}