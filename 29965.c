static uint8_t *smbXcli_iov_concat(TALLOC_CTX *mem_ctx,
				   const struct iovec *iov,
				   int count)
{
	ssize_t buflen;
	uint8_t *buf;

	buflen = iov_buflen(iov, count);
	if (buflen == -1) {
		return NULL;
	}

	buf = talloc_array(mem_ctx, uint8_t, buflen);
	if (buf == NULL) {
		return NULL;
	}

	iov_buf(iov, count, buf, buflen);

	return buf;
}
