ssize_t xdr_stream_decode_string(struct xdr_stream *xdr, char *str, size_t size)
{
	ssize_t ret;
	void *p;

	ret = xdr_stream_decode_opaque_inline(xdr, &p, size);
	if (ret > 0) {
		memcpy(str, p, ret);
		str[ret] = '\0';
		return strlen(str);
	}
	*str = '\0';
	return ret;
}