p11_rpc_buffer_add_byte_array (p11_buffer *buffer,
                               const unsigned char *data,
                               size_t length)
{
	if (data == NULL) {
		p11_rpc_buffer_add_uint32 (buffer, 0xffffffff);
		return;
	} else if (length >= 0x7fffffff) {
		p11_buffer_fail (buffer);
		return;
	}
	p11_rpc_buffer_add_uint32 (buffer, length);
	p11_buffer_add (buffer, data, length);
}