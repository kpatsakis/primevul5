static ssize_t extract_entropy(struct entropy_store *r, void *buf,
			       size_t nbytes, int min, int reserved)
{
	ssize_t ret = 0, i;
	__u8 tmp[EXTRACT_SIZE];

	xfer_secondary_pool(r, nbytes);
	nbytes = account(r, nbytes, min, reserved);

	while (nbytes) {
		extract_buf(r, tmp);
		i = min_t(int, nbytes, EXTRACT_SIZE);
		memcpy(buf, tmp, i);
		nbytes -= i;
		buf += i;
		ret += i;
	}

	/* Wipe data just returned from memory */
	memset(tmp, 0, sizeof(tmp));

	return ret;
}