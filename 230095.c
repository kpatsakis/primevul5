static int decode_opaque_inline(struct xdr_stream *xdr, unsigned int *len, char **string)
{
	ssize_t ret = xdr_stream_decode_opaque_inline(xdr, (void **)string,
			NFS4_OPAQUE_LIMIT);
	if (unlikely(ret < 0))
		return -EIO;
	*len = ret;
	return 0;
}