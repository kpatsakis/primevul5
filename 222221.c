p11_rpc_buffer_get_byte_array_value (p11_buffer *buffer,
				     size_t *offset,
				     void *value,
				     CK_ULONG *value_length)
{
	const unsigned char *val;
	size_t len;

	if (!p11_rpc_buffer_get_byte_array (buffer, offset, &val, &len))
		return false;

	if (val && value)
		memcpy (value, val, len);

	if (value_length)
		*value_length = len;

	return true;
}