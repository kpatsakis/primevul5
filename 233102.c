iasecc_read_public_key(struct sc_card *card, unsigned type,
		struct sc_path *key_path, unsigned ref, unsigned size,
		unsigned char **out, size_t *out_len)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo sdo;
	struct sc_pkcs15_bignum bn[2];
	struct sc_pkcs15_pubkey_rsa rsa_key;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (type != SC_ALGORITHM_RSA)
		LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);

	sc_log(ctx, "read public kay(ref:%i;size:%i)", ref, size);

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_RSA_PUBLIC;
	sdo.sdo_ref  = ref & ~IASECC_OBJECT_REF_LOCAL;

	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_RET(ctx, rv, "failed to read public key: cannot get RSA SDO data");

	if (out)
		*out = NULL;
	if (out_len)
		*out_len = 0;

	bn[0].data = (unsigned char *) malloc(sdo.data.pub_key.n.size);
	if (!bn[0].data)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "failed to read public key: cannot allocate modulus");
	bn[0].len = sdo.data.pub_key.n.size;
	memcpy(bn[0].data, sdo.data.pub_key.n.value, sdo.data.pub_key.n.size);

	bn[1].data = (unsigned char *) malloc(sdo.data.pub_key.e.size);
	if (!bn[1].data)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "failed to read public key: cannot allocate exponent");
	bn[1].len = sdo.data.pub_key.e.size;
	memcpy(bn[1].data, sdo.data.pub_key.e.value, sdo.data.pub_key.e.size);

	rsa_key.modulus = bn[0];
	rsa_key.exponent = bn[1];

	rv = sc_pkcs15_encode_pubkey_rsa(ctx, &rsa_key, out, out_len);
	LOG_TEST_RET(ctx, rv, "failed to read public key: cannot encode RSA public key");

	if (out && out_len)
		sc_log(ctx, "encoded public key: %s", sc_dump_hex(*out, *out_len));

	if (bn[0].data)
		free(bn[0].data);
	if (bn[1].data)
		free(bn[1].data);

	iasecc_sdo_free_fields(card, &sdo);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}