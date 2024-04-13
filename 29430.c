unpack_bz2_stream(transformer_state_t *xstate)
{
	IF_DESKTOP(long long total_written = 0;)
	bunzip_data *bd;
	char *outbuf;
	int i;
	unsigned len;

	if (check_signature16(xstate, BZIP2_MAGIC))
		return -1;

	outbuf = xmalloc(IOBUF_SIZE);
	len = 0;
	while (1) { /* "Process one BZ... stream" loop */

		i = start_bunzip(&bd, xstate->src_fd, outbuf + 2, len);

		if (i == 0) {
			while (1) { /* "Produce some output bytes" loop */
				i = read_bunzip(bd, outbuf, IOBUF_SIZE);
				if (i < 0) /* error? */
					break;
				i = IOBUF_SIZE - i; /* number of bytes produced */
				if (i == 0) /* EOF? */
					break;
				if (i != transformer_write(xstate, outbuf, i)) {
					i = RETVAL_SHORT_WRITE;
					goto release_mem;
				}
				IF_DESKTOP(total_written += i;)
			}
		}

		if (i != RETVAL_LAST_BLOCK
		/* Observed case when i == RETVAL_OK:
		 * "bzcat z.bz2", where "z.bz2" is a bzipped zero-length file
		 * (to be exact, z.bz2 is exactly these 14 bytes:
		 * 42 5a 68 39 17 72 45 38  50 90 00 00 00 00).
		 */
		 && i != RETVAL_OK
		) {
			bb_error_msg("bunzip error %d", i);
			break;
		}
		if (bd->headerCRC != bd->totalCRC) {
			bb_error_msg("CRC error");
			break;
		}

		/* Successfully unpacked one BZ stream */
		i = RETVAL_OK;

		/* Do we have "BZ..." after last processed byte?
		 * pbzip2 (parallelized bzip2) produces such files.
		 */
		len = bd->inbufCount - bd->inbufPos;
		memcpy(outbuf, &bd->inbuf[bd->inbufPos], len);
		if (len < 2) {
			if (safe_read(xstate->src_fd, outbuf + len, 2 - len) != 2 - len)
				break;
			len = 2;
		}
		if (*(uint16_t*)outbuf != BZIP2_MAGIC) /* "BZ"? */
			break;
		dealloc_bunzip(bd);
		len -= 2;
	}

 release_mem:
	dealloc_bunzip(bd);
	free(outbuf);

	return i ? i : IF_DESKTOP(total_written) + 0;
}
