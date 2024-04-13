get_input_token(unsigned char **buff_in, unsigned int buff_length)
{
	gss_buffer_t input_token;
	unsigned int len;

	if (g_get_tag_and_length(buff_in, OCTET_STRING, buff_length, &len) < 0)
		return (NULL);

	input_token = (gss_buffer_t)malloc(sizeof (gss_buffer_desc));
	if (input_token == NULL)
		return (NULL);

	input_token->length = len;
	if (input_token->length > 0) {
		input_token->value = gssalloc_malloc(input_token->length);
		if (input_token->value == NULL) {
			free(input_token);
			return (NULL);
		}

		memcpy(input_token->value, *buff_in, input_token->length);
	} else {
		input_token->value = NULL;
	}
	*buff_in += input_token->length;
	return (input_token);
}