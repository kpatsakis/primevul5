p11_rpc_buffer_get_attribute_array_value (p11_buffer *buffer,
					  size_t *offset,
					  void *value,
					  CK_ULONG *value_length)
{
	uint32_t count, i;
	CK_ATTRIBUTE *attr, temp;

	if (!p11_rpc_buffer_get_uint32 (buffer, offset, &count))
		return false;

	if (!value) {
		memset (&temp, 0, sizeof (CK_ATTRIBUTE));
		attr = &temp;
	} else
		attr = value;

	for (i = 0; i < count; i++) {
		if (!p11_rpc_buffer_get_attribute (buffer, offset, attr))
			return false;
		if (value)
			attr++;
	}

	if (value_length)
		*value_length = count * sizeof (CK_ATTRIBUTE);

	return true;
}