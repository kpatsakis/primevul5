const char *ntlmssp_t_str_i(const void *message, struct ntlmssp_buffer *buffer,
			    bool unicode)
{
	unsigned int len = read_le16(&buffer->length);
	const char *p = ((const char *) message) + read_le32(&buffer->offset);
	string_t *str;

	if (unicode)
		len /= sizeof(ucs2le_t);

	str = t_str_new(len);

	while (len-- > 0) {
		str_append_c(str, *p & 0x7f);
		p += unicode ? sizeof(ucs2le_t) : 1;
	}

	return str_c(str);
}