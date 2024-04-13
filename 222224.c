p11_rpc_buffer_set_uint16 (p11_buffer *buffer,
                           size_t offset,
                           uint16_t value)
{
	unsigned char *ptr;
	if (buffer->len < 2 || offset > buffer->len - 2) {
		p11_buffer_fail (buffer);
		return false;
	}
	ptr = (unsigned char *)buffer->data + offset;
	p11_rpc_buffer_encode_uint16 (ptr, value);
	return true;
}