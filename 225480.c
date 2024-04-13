put_input_token(unsigned char **buf_out, gss_buffer_t input_token,
		unsigned int buflen)
{
	int ret;

	/* if token length is 0, we do not want to send */
	if (input_token->length == 0)
		return (0);

	if (input_token->length > buflen)
		return (-1);

	*(*buf_out)++ = OCTET_STRING;
	if ((ret = gssint_put_der_length(input_token->length, buf_out,
			    input_token->length)))
		return (ret);
	TWRITE_STR(*buf_out, input_token->value, input_token->length);
	return (0);
}