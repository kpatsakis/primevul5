make_err_msg(const char *name)
{
	gss_buffer_desc buffer;

	if (name == NULL) {
		buffer.length = 0;
		buffer.value = NULL;
	} else {
		buffer.length = strlen(name)+1;
		buffer.value = make_spnego_token(name);
	}

	return (buffer);
}