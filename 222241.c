p11_rpc_buffer_add_mechanism_type_array_value (p11_buffer *buffer,
					       const void *value,
					       CK_ULONG value_length)
{
	size_t count = value_length / sizeof (CK_MECHANISM_TYPE);

	/* Check if count can be converted to uint32_t. */
	if (count > UINT32_MAX) {
		p11_buffer_fail (buffer);
		return;
	}

	/* Write the number of items */
	p11_rpc_buffer_add_uint32 (buffer, count);

	if (value) {
		const CK_MECHANISM_TYPE *mechs = value;
		size_t i;

		for (i = 0; i < count; i++) {
			if (mechs[i] > UINT64_MAX) {
				p11_buffer_fail (buffer);
				return;
			}
			p11_rpc_buffer_add_uint64 (buffer, mechs[i]);
		}
	}
}