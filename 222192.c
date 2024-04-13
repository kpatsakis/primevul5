p11_rpc_buffer_get_ulong_value (p11_buffer *buffer,
				size_t *offset,
				void *value,
				CK_ULONG *value_length)
{
	uint64_t val;

	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val))
		return false;

	if (value) {
		CK_ULONG ulong_value = val;
		memcpy (value, &ulong_value, sizeof (CK_ULONG));
	}

	if (value_length)
		*value_length = sizeof (CK_ULONG);

	return true;
}