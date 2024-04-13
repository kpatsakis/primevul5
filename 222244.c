p11_rpc_buffer_get_mechanism_type_array_value (p11_buffer *buffer,
					       size_t *offset,
					       void *value,
					       CK_ULONG *value_length)
{
	uint32_t count, i;
	CK_MECHANISM_TYPE *mech, temp;

	if (!p11_rpc_buffer_get_uint32 (buffer, offset, &count))
		return false;

	if (!value) {
		memset (&temp, 0, sizeof (CK_MECHANISM_TYPE));
		mech = &temp;
	} else
		mech = value;

	for (i = 0; i < count; i++) {
		CK_ULONG len;
		if (!p11_rpc_buffer_get_ulong_value (buffer, offset, mech, &len))
			return false;
		if (value)
			mech++;
	}

	if (value_length)
		*value_length = count * sizeof (CK_MECHANISM_TYPE);

	return true;
}