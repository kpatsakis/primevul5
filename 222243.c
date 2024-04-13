p11_rpc_buffer_add_uint16 (p11_buffer *buffer,
                           uint16_t value)
{
	size_t offset = buffer->len;
	if (!p11_buffer_append (buffer, 2))
		return_if_reached ();
	p11_rpc_buffer_set_uint16 (buffer, offset, value);
}