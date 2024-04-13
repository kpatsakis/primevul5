g_token_size(gss_OID_const mech, unsigned int body_size)
{
	int hdrsize;

	/*
	 * Initialize the header size to the
	 * MECH_OID byte + the bytes needed to indicate the
	 * length of the OID + the OID itself.
	 *
	 * 0x06 [MECHLENFIELD] MECHDATA
	 */
	hdrsize = 1 + gssint_der_length_size(mech->length) + mech->length;

	/*
	 * Now add the bytes needed for the initial header
	 * token bytes:
	 * 0x60 + [DER_LEN] + HDRSIZE
	 */
	hdrsize += 1 + gssint_der_length_size(body_size + hdrsize);

	return (hdrsize + body_size);
}