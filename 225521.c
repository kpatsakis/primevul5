get_req_flags(unsigned char **buff_in, OM_uint32 bodysize,
	      OM_uint32 *req_flags)
{
	unsigned int len;

	if (**buff_in != (CONTEXT | 0x01))
		return (0);

	if (g_get_tag_and_length(buff_in, (CONTEXT | 0x01),
				bodysize, &len) < 0)
		return GSS_S_DEFECTIVE_TOKEN;

	if (*(*buff_in)++ != BIT_STRING)
		return GSS_S_DEFECTIVE_TOKEN;

	if (*(*buff_in)++ != BIT_STRING_LENGTH)
		return GSS_S_DEFECTIVE_TOKEN;

	if (*(*buff_in)++ != BIT_STRING_PADDING)
		return GSS_S_DEFECTIVE_TOKEN;

	*req_flags = (OM_uint32) (*(*buff_in)++ >> 1);
	return (0);
}