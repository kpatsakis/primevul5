make_printable_string(unsigned etype, const gnutls_datum_t * input,
		      gnutls_datum_t * out)
{
	int printable = 0;
	int ret;
	unsigned int i;
	size_t size;

	if (etype == ASN1_ETYPE_BMP_STRING) {
		ret = _gnutls_ucs2_to_utf8(input->data, input->size, out);
		if (ret < 0) {
			/* could not convert. Handle it as non-printable */
			printable = 0;
		} else
			printable = 1;
	} else if (etype == ASN1_ETYPE_TELETEX_STRING) {
		int ascii = 0;
		/* HACK: if the teletex string contains only ascii
		 * characters then treat it as printable.
		 */
		for (i = 0; i < input->size; i++)
			if (!c_isascii(input->data[i]))
				ascii = 1;

		if (ascii == 0) {
			out->data = gnutls_malloc(input->size + 1);
			if (out->data == NULL)
				return
				    gnutls_assert_val
				    (GNUTLS_E_MEMORY_ERROR);

			memcpy(out->data, input->data, input->size);
			out->size = input->size;

			out->data[out->size] = 0;

			printable = 1;
		}
	} else if (etype != ASN1_ETYPE_UNIVERSAL_STRING)	/* supported but not printable */
		return GNUTLS_E_INVALID_REQUEST;

	if (printable == 0) {	/* need to allocate out */
		out->size = input->size * 2 + 2;
		out->data = gnutls_malloc(out->size);
		if (out->data == NULL)
			return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

		size = out->size;
		ret = data2hex(input->data, input->size, out->data, &size);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}
		out->size = size;
	}

	return 0;

      cleanup:
	_gnutls_free_datum(out);
	return ret;
}