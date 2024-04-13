p11_rpc_buffer_add_attribute_array_value (p11_buffer *buffer,
					  const void *value,
					  CK_ULONG value_length)
{
	const CK_ATTRIBUTE *attrs = value;
	size_t count = value_length / sizeof (CK_ATTRIBUTE);
	size_t i;

	/* Check if count can be converted to uint32_t. */
	if (count > UINT32_MAX) {
		p11_buffer_fail (buffer);
		return;
	}

	/* Write the number of items */
	p11_rpc_buffer_add_uint32 (buffer, count);

	/* Actually write the attributes.  */
	for (i = 0; i < count; i++) {
		const CK_ATTRIBUTE *attr = &(attrs[i]);
		p11_rpc_buffer_add_attribute (buffer, attr);
	}
}