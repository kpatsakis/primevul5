int _gnutls_copy_string(gnutls_datum_t* str, uint8_t *out, size_t *out_size)
{
unsigned size_to_check;

	size_to_check = str->size + 1;

	if ((unsigned) size_to_check > *out_size) {
		gnutls_assert();
		(*out_size) = size_to_check;
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}

	if (out != NULL && str->data != NULL) {
		memcpy(out, str->data, str->size);
		out[str->size] = 0;
	} else if (out != NULL) {
		out[0] = 0;
	}
	*out_size = str->size;

	return 0;
}