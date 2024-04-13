_gnutls_check_valid_key_id(gnutls_datum_t *key_id,
                           gnutls_x509_crt_t cert, time_t now)
{
	uint8_t id[MAX_KEY_ID_SIZE];
	size_t id_size;
	bool result = 0;

	if (now > gnutls_x509_crt_get_expiration_time(cert) ||
	    now < gnutls_x509_crt_get_activation_time(cert)) {
		/* don't bother, certificate is not yet activated or expired */
		gnutls_assert();
		goto out;
	}

	id_size = sizeof(id);
	if (gnutls_x509_crt_get_subject_key_id(cert, id, &id_size, NULL) < 0) {
		gnutls_assert();
		goto out;
	}

	if (id_size == key_id->size && !memcmp(id, key_id->data, id_size))
		result = 1;

 out:
	return result;
}