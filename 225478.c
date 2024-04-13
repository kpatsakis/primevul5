get_negTokenInit(OM_uint32 *minor_status,
		 gss_buffer_t buf,
		 gss_buffer_t der_mechSet,
		 gss_OID_set *mechSet,
		 OM_uint32 *req_flags,
		 gss_buffer_t *mechtok,
		 gss_buffer_t *mechListMIC)
{
	OM_uint32 err;
	unsigned char *ptr, *bufstart;
	unsigned int len;
	gss_buffer_desc tmpbuf;

	*minor_status = 0;
	der_mechSet->length = 0;
	der_mechSet->value = NULL;
	*mechSet = GSS_C_NO_OID_SET;
	*req_flags = 0;
	*mechtok = *mechListMIC = GSS_C_NO_BUFFER;

	ptr = bufstart = buf->value;
	if ((buf->length - (ptr - bufstart)) > INT_MAX)
		return GSS_S_FAILURE;
#define REMAIN (buf->length - (ptr - bufstart))

	err = g_verify_token_header(gss_mech_spnego,
				    &len, &ptr, 0, REMAIN);
	if (err) {
		*minor_status = err;
		map_errcode(minor_status);
		return GSS_S_FAILURE;
	}
	*minor_status = g_verify_neg_token_init(&ptr, REMAIN);
	if (*minor_status) {
		map_errcode(minor_status);
		return GSS_S_FAILURE;
	}

	/* alias into input_token */
	tmpbuf.value = ptr;
	tmpbuf.length = REMAIN;
	*mechSet = get_mech_set(minor_status, &ptr, REMAIN);
	if (*mechSet == NULL)
		return GSS_S_FAILURE;

	tmpbuf.length = ptr - (unsigned char *)tmpbuf.value;
	der_mechSet->value = gssalloc_malloc(tmpbuf.length);
	if (der_mechSet->value == NULL)
		return GSS_S_FAILURE;
	memcpy(der_mechSet->value, tmpbuf.value, tmpbuf.length);
	der_mechSet->length = tmpbuf.length;

	err = get_req_flags(&ptr, REMAIN, req_flags);
	if (err != GSS_S_COMPLETE) {
		return err;
	}
	if (g_get_tag_and_length(&ptr, (CONTEXT | 0x02),
				 REMAIN, &len) >= 0) {
		*mechtok = get_input_token(&ptr, len);
		if (*mechtok == GSS_C_NO_BUFFER) {
			return GSS_S_FAILURE;
		}
	}
	if (g_get_tag_and_length(&ptr, (CONTEXT | 0x03),
				 REMAIN, &len) >= 0) {
		*mechListMIC = get_input_token(&ptr, len);
		if (*mechListMIC == GSS_C_NO_BUFFER) {
			return GSS_S_FAILURE;
		}
	}
	return GSS_S_COMPLETE;
#undef REMAIN
}