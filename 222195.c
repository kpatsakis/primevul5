p11_rpc_buffer_get_byte_value (p11_buffer *buffer,
			       size_t *offset,
			       void *value,
			       CK_ULONG *value_length)
{
	unsigned char val;

	if (!p11_rpc_buffer_get_byte (buffer, offset, &val))
		return false;

	if (value) {
		CK_BYTE byte_value = val;
		memcpy (value, &byte_value, sizeof (CK_BYTE));
	}

	if (value_length)
		*value_length = sizeof (CK_BYTE);

	return true;
}