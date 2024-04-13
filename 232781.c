_gnutls_x509_export_int_named(ASN1_TYPE asn1_data, const char *name,
			      gnutls_x509_crt_fmt_t format,
			      const char *pem_header,
			      unsigned char *output_data,
			      size_t * output_data_size)
{
	int ret;
	gnutls_datum_t out = {NULL,0};
	size_t size;

	ret = _gnutls_x509_export_int_named2(asn1_data, name,
					     format, pem_header, &out);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (format == GNUTLS_X509_FMT_PEM)
		size = out.size + 1;
	else
		size = out.size;

	if (*output_data_size < size) {
		*output_data_size = size;
		ret = gnutls_assert_val(GNUTLS_E_SHORT_MEMORY_BUFFER);
		goto cleanup;
	}

	*output_data_size = (size_t) out.size;
	if (output_data) {
		memcpy(output_data, out.data, (size_t) out.size);
		if (format == GNUTLS_X509_FMT_PEM)
			output_data[out.size] = 0;
	}

	ret = 0;

      cleanup:
	gnutls_free(out.data);

	return ret;
}