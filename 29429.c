int FAST_FUNC start_bunzip(bunzip_data **bdp, int in_fd,
		const void *inbuf, int len)
{
	bunzip_data *bd;
	unsigned i;
	enum {
		BZh0 = ('B' << 24) + ('Z' << 16) + ('h' << 8) + '0',
		h0 = ('h' << 8) + '0',
	};

	/* Figure out how much data to allocate */
	i = sizeof(bunzip_data);
	if (in_fd != -1) i += IOBUF_SIZE;

	/* Allocate bunzip_data.  Most fields initialize to zero. */
	bd = *bdp = xzalloc(i);

	/* Setup input buffer */
	bd->in_fd = in_fd;
	if (-1 == in_fd) {
		/* in this case, bd->inbuf is read-only */
		bd->inbuf = (void*)inbuf; /* cast away const-ness */
	} else {
		bd->inbuf = (uint8_t*)(bd + 1);
		memcpy(bd->inbuf, inbuf, len);
	}
	bd->inbufCount = len;

	/* Init the CRC32 table (big endian) */
	crc32_filltable(bd->crc32Table, 1);

	/* Setup for I/O error handling via longjmp */
	i = setjmp(bd->jmpbuf);
	if (i) return i;

	/* Ensure that file starts with "BZh['1'-'9']." */
	/* Update: now caller verifies 1st two bytes, makes .gz/.bz2
	 * integration easier */
	/* was: */
	/* i = get_bits(bd, 32); */
	/* if ((unsigned)(i - BZh0 - 1) >= 9) return RETVAL_NOT_BZIP_DATA; */
	i = get_bits(bd, 16);
	if ((unsigned)(i - h0 - 1) >= 9) return RETVAL_NOT_BZIP_DATA;

	/* Fourth byte (ascii '1'-'9') indicates block size in units of 100k of
	   uncompressed data.  Allocate intermediate buffer for block. */
	/* bd->dbufSize = 100000 * (i - BZh0); */
	bd->dbufSize = 100000 * (i - h0);

	/* Cannot use xmalloc - may leak bd in NOFORK case! */
	bd->dbuf = malloc_or_warn(bd->dbufSize * sizeof(bd->dbuf[0]));
	if (!bd->dbuf) {
		free(bd);
		xfunc_die();
	}
	return RETVAL_OK;
}
