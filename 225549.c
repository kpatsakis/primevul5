get_mech_oid(OM_uint32 *minor_status, unsigned char **buff_in, size_t length)
{
	OM_uint32	status;
	gss_OID_desc 	toid;
	gss_OID		mech_out = NULL;
	unsigned char		*start, *end;

	if (length < 1 || **buff_in != MECH_OID)
		return (NULL);

	start = *buff_in;
	end = start + length;

	(*buff_in)++;
	toid.length = *(*buff_in)++;

	if ((*buff_in + toid.length) > end)
		return (NULL);

	toid.elements = *buff_in;
	*buff_in += toid.length;

	status = generic_gss_copy_oid(minor_status, &toid, &mech_out);

	if (status != GSS_S_COMPLETE) {
		map_errcode(minor_status);
		mech_out = NULL;
	}

	return (mech_out);
}