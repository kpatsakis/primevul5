g_verify_neg_token_init(unsigned char **buf_in, unsigned int cur_size)
{
	unsigned char *buf = *buf_in;
	unsigned char *endptr = buf + cur_size;
	int seqsize;
	int ret = 0;
	unsigned int bytes;

	/*
	 * Verify this is a NegotiationToken type token
	 * - check for a0(context specific identifier)
	 * - get length and verify that enoughd ata exists
	 */
	if (g_get_tag_and_length(&buf, CONTEXT, cur_size, &bytes) < 0)
		return (G_BAD_TOK_HEADER);

	cur_size = bytes; /* should indicate bytes remaining */

	/*
	 * Verify the next piece, it should identify this as
	 * a strucure of type NegTokenInit.
	 */
	if (*buf++ == SEQUENCE) {
		if ((seqsize = gssint_get_der_length(&buf, cur_size, &bytes)) < 0)
			return (G_BAD_TOK_HEADER);
		/*
		 * Make sure we have the entire buffer as described
		 */
		if (seqsize > endptr - buf)
			return (G_BAD_TOK_HEADER);
	} else {
		return (G_BAD_TOK_HEADER);
	}

	cur_size = seqsize; /* should indicate bytes remaining */

	/*
	 * Verify that the first blob is a sequence of mechTypes
	 */
	if (*buf++ == CONTEXT) {
		if ((seqsize = gssint_get_der_length(&buf, cur_size, &bytes)) < 0)
			return (G_BAD_TOK_HEADER);
		/*
		 * Make sure we have the entire buffer as described
		 */
		if (seqsize > endptr - buf)
			return (G_BAD_TOK_HEADER);
	} else {
		return (G_BAD_TOK_HEADER);
	}

	/*
	 * At this point, *buf should be at the beginning of the
	 * DER encoded list of mech types that are to be negotiated.
	 */
	*buf_in = buf;

	return (ret);

}