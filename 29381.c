static void xdr_init(struct xdr_stream *stream, void *buf, int len)
{
	stream->p = stream->buf = buf;
	stream->end = stream->buf + len;
}
