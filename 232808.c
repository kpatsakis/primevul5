int _gnutls_strdatum_to_buf(gnutls_datum_t * d, void *buf,
			    size_t * buf_size)
{
	int ret;
	uint8_t *_buf = buf;

	if (buf == NULL || *buf_size < d->size + 1) {
		*buf_size = d->size + 1;
		ret = gnutls_assert_val(GNUTLS_E_SHORT_MEMORY_BUFFER);
		goto cleanup;
	}
	memcpy(buf, d->data, d->size);
	_buf[d->size] = 0;

	*buf_size = d->size;
	ret = 0;

      cleanup:
	_gnutls_free_datum(d);

	return ret;
}