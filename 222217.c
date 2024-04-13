p11_rpc_buffer_add_byte_value (p11_buffer *buffer,
			       const void *value,
			       CK_ULONG value_length)
{
	CK_BYTE byte_value = 0;

	/* Check if value can be converted to CK_BYTE. */
	if (value_length > sizeof (CK_BYTE)) {
		p11_buffer_fail (buffer);
		return;
	}
	if (value)
		memcpy (&byte_value, value, value_length);

	p11_rpc_buffer_add_byte (buffer, byte_value);
}