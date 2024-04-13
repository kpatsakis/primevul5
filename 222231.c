p11_rpc_buffer_get_date_value (p11_buffer *buffer,
			       size_t *offset,
			       void *value,
			       CK_ULONG *value_length)
{
	CK_DATE date_value;
	const unsigned char *array;
	size_t array_length;

	/* The encoded date may be empty. */
	if (!p11_rpc_buffer_get_byte_array (buffer, offset,
					    &array, &array_length) ||
	    (array_length != 0 && array_length != sizeof (CK_DATE)))
		return false;

	if (value && array_length == sizeof (CK_DATE)) {
		memcpy (date_value.year, array, 4);
		memcpy (date_value.month, array + 4, 2);
		memcpy (date_value.day, array + 6, 2);
		memcpy (value, &date_value, sizeof (CK_DATE));
	}

	if (value_length)
		*value_length = array_length;

	return true;
}