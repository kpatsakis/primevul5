p11_rpc_buffer_add_uint32 (p11_buffer *buffer,
                           uint32_t value)
{
	size_t offset = buffer->len;
	if (!p11_buffer_append (buffer, 4))
		return_val_if_reached ();
	p11_rpc_buffer_set_uint32 (buffer, offset, value);
}