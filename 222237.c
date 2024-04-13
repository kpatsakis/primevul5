p11_rpc_buffer_set_uint32 (p11_buffer *buffer,
                           size_t offset,
                           uint32_t value)
{
	unsigned char *ptr;
	if (buffer->len < 4 || offset > buffer->len - 4) {
		p11_buffer_fail (buffer);
		return false;
	}
	ptr = (unsigned char*)buffer->data + offset;
	p11_rpc_buffer_encode_uint32 (ptr, value);
	return true;
}