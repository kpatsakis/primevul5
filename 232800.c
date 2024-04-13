static int str_escape(const gnutls_datum_t * str, gnutls_datum_t * escaped)
{
	unsigned int j, i;
	uint8_t *buffer = NULL;
	int ret;

	if (str == NULL)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	/* the string will be at most twice the original */
	buffer = gnutls_malloc(str->size * 2 + 2);
	if (buffer == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	for (i = j = 0; i < str->size; i++) {
		if (str->data[i] == 0) {
			/* this is handled earlier */
			ret = gnutls_assert_val(GNUTLS_E_ASN1_DER_ERROR);
			goto cleanup;
		}

		if (str->data[i] == ',' || str->data[i] == '+'
		    || str->data[i] == '"' || str->data[i] == '\\'
		    || str->data[i] == '<' || str->data[i] == '>'
		    || str->data[i] == ';' || str->data[i] == 0)
			buffer[j++] = '\\';
		else if (i == 0 && str->data[i] == '#')
			buffer[j++] = '\\';
		else if (i == 0 && str->data[i] == ' ')
			buffer[j++] = '\\';
		else if (i == (str->size - 1) && str->data[i] == ' ')
			buffer[j++] = '\\';

		buffer[j++] = str->data[i];
	}

	/* null terminate the string */
	buffer[j] = 0;
	escaped->data = buffer;
	escaped->size = j;

	return 0;
      cleanup:
	gnutls_free(buffer);
	return ret;
}