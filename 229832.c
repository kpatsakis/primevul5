static int decode_op_hdr(struct xdr_stream *xdr, enum nfs_opnum4 expected)
{
	int retval;

	__decode_op_hdr(xdr, expected, &retval);
	return retval;
}