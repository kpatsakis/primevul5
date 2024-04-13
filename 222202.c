p11_rpc_buffer_get_uint32 (p11_buffer *buf,
                           size_t *offset,
                           uint32_t *value)
{
	unsigned char *ptr;
	if (buf->len < 4 || *offset > buf->len - 4) {
		p11_buffer_fail (buf);
		return false;
	}
	ptr = (unsigned char*)buf->data + *offset;
	if (value != NULL)
		*value = p11_rpc_buffer_decode_uint32 (ptr);
	*offset = *offset + 4;
	return true;
}