g_verify_token_header(gss_OID_const mech,
		    unsigned int *body_size,
		    unsigned char **buf_in,
		    int tok_type,
		    unsigned int toksize)
{
	unsigned char *buf = *buf_in;
	int seqsize;
	gss_OID_desc toid;
	int ret = 0;
	unsigned int bytes;

	if (toksize-- < 1)
		return (G_BAD_TOK_HEADER);

	if (*buf++ != HEADER_ID)
		return (G_BAD_TOK_HEADER);

	if ((seqsize = gssint_get_der_length(&buf, toksize, &bytes)) < 0)
		return (G_BAD_TOK_HEADER);

	if ((seqsize + bytes) != toksize)
		return (G_BAD_TOK_HEADER);

	if (toksize-- < 1)
		return (G_BAD_TOK_HEADER);


	if (*buf++ != MECH_OID)
		return (G_BAD_TOK_HEADER);

	if (toksize-- < 1)
		return (G_BAD_TOK_HEADER);

	toid.length = *buf++;

	if (toksize < toid.length)
		return (G_BAD_TOK_HEADER);
	else
		toksize -= toid.length;

	toid.elements = buf;
	buf += toid.length;

	if (!g_OID_equal(&toid, mech))
		ret = G_WRONG_MECH;

	/*
	 * G_WRONG_MECH is not returned immediately because it's more important
	 * to return G_BAD_TOK_HEADER if the token header is in fact bad
	 */
	if (toksize < 2)
		return (G_BAD_TOK_HEADER);
	else
		toksize -= 2;

	if (!ret) {
		*buf_in = buf;
		*body_size = toksize;
	}

	return (ret);
}