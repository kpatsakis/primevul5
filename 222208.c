p11_rpc_buffer_add_ulong_value (p11_buffer *buffer,
				const void *value,
				CK_ULONG value_length)
{
	CK_ULONG ulong_value = 0;

	/* Check if value can be converted to CK_ULONG. */
	if (value_length > sizeof (CK_ULONG)) {
		p11_buffer_fail (buffer);
		return;
	}
	if (value)
		memcpy (&ulong_value, value, value_length);

	/* Check if ulong_value can be converted to uint64_t. */
	if (ulong_value > UINT64_MAX) {
		p11_buffer_fail (buffer);
		return;
	}

	p11_rpc_buffer_add_uint64 (buffer, ulong_value);
}