int _gnutls_copy_data(gnutls_datum_t* str, uint8_t *out, size_t *out_size)
{
	if ((unsigned) str->size > *out_size) {
		gnutls_assert();
		(*out_size) = str->size;
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}

	if (out != NULL && str->data != NULL) {
		memcpy(out, str->data, str->size);
	}
	*out_size = str->size;

	return 0;
}