static int decode_setattr(struct xdr_stream *xdr)
{
	int status;

	status = decode_op_hdr(xdr, OP_SETATTR);
	if (status)
		return status;
	if (decode_bitmap4(xdr, NULL, 0) >= 0)
		return 0;
	return -EIO;
}