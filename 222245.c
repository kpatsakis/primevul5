p11_rpc_buffer_add_byte_array_value (p11_buffer *buffer,
				     const void *value,
				     CK_ULONG value_length)
{
	/* Check if value length can be converted to uint32_t, as
	 * p11_rpc_buffer_add_byte_array expects. */
	if (value_length > UINT32_MAX) {
		p11_buffer_fail (buffer);
		return;
	}

	p11_rpc_buffer_add_byte_array (buffer, value, value_length);
}