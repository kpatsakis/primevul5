p11_rpc_buffer_get_uint16 (p11_buffer *buf,
                           size_t *offset,
                           uint16_t *value)
{
	unsigned char *ptr;
	if (buf->len < 2 || *offset > buf->len - 2) {
		p11_buffer_fail (buf);
		return false;
	}
	ptr = (unsigned char*)buf->data + *offset;
	if (value != NULL)
		*value = p11_rpc_buffer_decode_uint16 (ptr);
	*offset = *offset + 2;
	return true;
}