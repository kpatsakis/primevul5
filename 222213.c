p11_rpc_buffer_add_date_value (p11_buffer *buffer,
			       const void *value,
			       CK_ULONG value_length)
{
	CK_DATE date_value;
	unsigned char array[8];
	unsigned char *ptr = NULL;

	/* Check if value is empty or can be converted to CK_DATE. */
	if (value_length != 0 && value_length != sizeof (CK_DATE)) {
		p11_buffer_fail (buffer);
		return;
	}

	if (value && value_length == sizeof (CK_DATE)) {
		memcpy (&date_value, value, value_length);
		memcpy (array, date_value.year, 4);
		memcpy (array + 4, date_value.month, 2);
		memcpy (array + 6, date_value.day, 2);
		ptr = array;
	}

	p11_rpc_buffer_add_byte_array (buffer, ptr, value_length);
}