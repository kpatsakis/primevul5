put_mech_oid(unsigned char **buf_out, gss_OID_const mech, unsigned int buflen)
{
	if (buflen < mech->length + 2)
		return (-1);
	*(*buf_out)++ = MECH_OID;
	*(*buf_out)++ = (unsigned char) mech->length;
	memcpy(*buf_out, mech->elements, mech->length);
	*buf_out += mech->length;
	return (0);
}