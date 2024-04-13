g_make_token_header(gss_OID_const mech,
		    unsigned int body_size,
		    unsigned char **buf,
		    unsigned int totallen)
{
	int ret = 0;
	unsigned int hdrsize;
	unsigned char *p = *buf;

	hdrsize = 1 + gssint_der_length_size(mech->length) + mech->length;

	*(*buf)++ = HEADER_ID;
	if ((ret = gssint_put_der_length(hdrsize + body_size, buf, totallen)))
		return (ret);

	*(*buf)++ = MECH_OID;
	if ((ret = gssint_put_der_length(mech->length, buf,
			    totallen - (int)(p - *buf))))
		return (ret);
	TWRITE_STR(*buf, mech->elements, mech->length);
	return (0);
}