get_negTokenResp(OM_uint32 *minor_status,
		 unsigned char *buf, unsigned int buflen,
		 OM_uint32 *negState,
		 gss_OID *supportedMech,
		 gss_buffer_t *responseToken,
		 gss_buffer_t *mechListMIC)
{
	unsigned char *ptr, *bufstart;
	unsigned int len;
	int tmplen;
	unsigned int tag, bytes;

	*negState = ACCEPT_DEFECTIVE_TOKEN;
	*supportedMech = GSS_C_NO_OID;
	*responseToken = *mechListMIC = GSS_C_NO_BUFFER;
	ptr = bufstart = buf;
#define REMAIN (buflen - (ptr - bufstart))

	if (g_get_tag_and_length(&ptr, (CONTEXT | 0x01), REMAIN, &len) < 0)
		return GSS_S_DEFECTIVE_TOKEN;
	if (*ptr++ == SEQUENCE) {
		tmplen = gssint_get_der_length(&ptr, REMAIN, &bytes);
		if (tmplen < 0 || REMAIN < (unsigned int)tmplen)
			return GSS_S_DEFECTIVE_TOKEN;
	}
	if (REMAIN < 1)
		tag = 0;
	else
		tag = *ptr++;

	if (tag == CONTEXT) {
		tmplen = gssint_get_der_length(&ptr, REMAIN, &bytes);
		if (tmplen < 0 || REMAIN < (unsigned int)tmplen)
			return GSS_S_DEFECTIVE_TOKEN;

		if (g_get_tag_and_length(&ptr, ENUMERATED,
					 REMAIN, &len) < 0)
			return GSS_S_DEFECTIVE_TOKEN;

		if (len != ENUMERATION_LENGTH)
			return GSS_S_DEFECTIVE_TOKEN;

		if (REMAIN < 1)
			return GSS_S_DEFECTIVE_TOKEN;
		*negState = *ptr++;

		if (REMAIN < 1)
			tag = 0;
		else
			tag = *ptr++;
	}
	if (tag == (CONTEXT | 0x01)) {
		tmplen = gssint_get_der_length(&ptr, REMAIN, &bytes);
		if (tmplen < 0 || REMAIN < (unsigned int)tmplen)
			return GSS_S_DEFECTIVE_TOKEN;

		*supportedMech = get_mech_oid(minor_status, &ptr, REMAIN);
		if (*supportedMech == GSS_C_NO_OID)
			return GSS_S_DEFECTIVE_TOKEN;

		if (REMAIN < 1)
			tag = 0;
		else
			tag = *ptr++;
	}
	if (tag == (CONTEXT | 0x02)) {
		tmplen = gssint_get_der_length(&ptr, REMAIN, &bytes);
		if (tmplen < 0 || REMAIN < (unsigned int)tmplen)
			return GSS_S_DEFECTIVE_TOKEN;

		*responseToken = get_input_token(&ptr, REMAIN);
		if (*responseToken == GSS_C_NO_BUFFER)
			return GSS_S_DEFECTIVE_TOKEN;

		if (REMAIN < 1)
			tag = 0;
		else
			tag = *ptr++;
	}
	if (tag == (CONTEXT | 0x03)) {
		tmplen = gssint_get_der_length(&ptr, REMAIN, &bytes);
		if (tmplen < 0 || REMAIN < (unsigned int)tmplen)
			return GSS_S_DEFECTIVE_TOKEN;

		*mechListMIC = get_input_token(&ptr, REMAIN);
		if (*mechListMIC == GSS_C_NO_BUFFER)
			return GSS_S_DEFECTIVE_TOKEN;

                /* Handle Windows 2000 duplicate response token */
                if (*responseToken &&
                    ((*responseToken)->length == (*mechListMIC)->length) &&
                    !memcmp((*responseToken)->value, (*mechListMIC)->value,
                            (*responseToken)->length)) {
			OM_uint32 tmpmin;

			gss_release_buffer(&tmpmin, *mechListMIC);
			free(*mechListMIC);
			*mechListMIC = NULL;
		}
	}
	return GSS_S_COMPLETE;
#undef REMAIN
}