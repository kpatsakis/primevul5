data2hex(const void *data, size_t data_size,
	 void *_out, size_t * sizeof_out)
{
	char *res;
	char escaped[MAX_STRING_LEN];
	unsigned int size, res_size;
	char *out = _out;

	if (2 * data_size + 1 > MAX_STRING_LEN) {
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}

	res =
	    _gnutls_bin2hex(data, data_size, escaped, sizeof(escaped),
			    NULL);
	if (!res) {
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}

	res_size = strlen(res);
	size = res_size + 1;	/* +1 for the '#' */
	if (size + 1 > *sizeof_out) {
		*sizeof_out = size + 1;
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}
	*sizeof_out = size;	/* -1 for the null +1 for the '#' */

	if (out) {
		out[0] = '#';
		memcpy(&out[1], res, res_size);
		out[size] = 0;
	}

	return 0;
}