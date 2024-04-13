p11_rpc_buffer_get_byte (p11_buffer *buf,
                         size_t *offset,
                         unsigned char *val)
{
	unsigned char *ptr;
	if (buf->len < 1 || *offset > buf->len - 1) {
		p11_buffer_fail (buf);
		return 0;
	}
	ptr = (unsigned char *)buf->data + *offset;
	if (val != NULL)
		*val = *ptr;
	*offset = *offset + 1;
	return 1;
}