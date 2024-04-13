int ssl3_peek(SSL *s, void *buf, int len)
	{
	return ssl3_read_internal(s, buf, len, 1);
	}
