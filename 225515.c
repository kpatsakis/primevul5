get_mech_set(OM_uint32 *minor_status, unsigned char **buff_in,
	     unsigned int buff_length)
{
	gss_OID_set returned_mechSet;
	OM_uint32 major_status;
	int length;
	unsigned int bytes;
	OM_uint32 set_length;
	unsigned char		*start;
	int i;

	if (**buff_in != SEQUENCE_OF)
		return (NULL);

	start = *buff_in;
	(*buff_in)++;

	length = gssint_get_der_length(buff_in, buff_length, &bytes);
	if (length < 0 || buff_length - bytes < (unsigned int)length)
		return NULL;

	major_status = gss_create_empty_oid_set(minor_status,
						&returned_mechSet);
	if (major_status != GSS_S_COMPLETE)
		return (NULL);

	for (set_length = 0, i = 0; set_length < (unsigned int)length; i++) {
		gss_OID_desc *temp = get_mech_oid(minor_status, buff_in,
			buff_length - (*buff_in - start));
		if (temp == NULL)
			break;

		major_status = gss_add_oid_set_member(minor_status,
						      temp, &returned_mechSet);
		if (major_status == GSS_S_COMPLETE) {
			set_length += returned_mechSet->elements[i].length +2;
			if (generic_gss_release_oid(minor_status, &temp))
				map_errcode(minor_status);
		}
	}

	return (returned_mechSet);
}