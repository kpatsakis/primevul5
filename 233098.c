iasecc_compute_signature_dst(struct sc_card *card,
		const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_private_data *prv = (struct iasecc_private_data *) card->drv_data;
	struct sc_security_env *env = &prv->security_env;
	struct iasecc_qsign_data qsign_data;
	struct sc_apdu apdu;
	size_t offs = 0, hash_len = 0;
	unsigned char sbuf[SC_MAX_APDU_BUFFER_SIZE];
	unsigned char rbuf[SC_MAX_APDU_BUFFER_SIZE];
	int rv = SC_SUCCESS;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "iasecc_compute_signature_dst() input length %"SC_FORMAT_LEN_SIZE_T"u",
	       in_len);
	if (env->operation != SC_SEC_OPERATION_SIGN)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "It's not SC_SEC_OPERATION_SIGN");
	else if (!(prv->key_size & 0x1E0) || (prv->key_size & ~0x1E0))
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Invalid key size for SC_SEC_OPERATION_SIGN");

	memset(&qsign_data, 0, sizeof(qsign_data));
	if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA1)   {
		rv = iasecc_qsign_data_sha1(card->ctx, in, in_len, &qsign_data);
	}
	else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA256)   {
#if OPENSSL_VERSION_NUMBER >= 0x00908000L
		rv = iasecc_qsign_data_sha256(card->ctx, in, in_len, &qsign_data);
#else
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "SHA256 is not supported by OpenSSL previous to v0.9.8");
#endif
	}
	else
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Need RSA_HASH_SHA1 or RSA_HASH_SHA256 algorithm");
	LOG_TEST_RET(ctx, rv, "Cannot get QSign data");

	sc_log(ctx,
	       "iasecc_compute_signature_dst() hash_len %"SC_FORMAT_LEN_SIZE_T"u; key_size %"SC_FORMAT_LEN_SIZE_T"u",
	       hash_len, prv->key_size);

	memset(sbuf, 0, sizeof(sbuf));
	sbuf[offs++] = 0x90;
	if (qsign_data.counter_long)   {
		sbuf[offs++] = qsign_data.hash_size + 8;
		memcpy(sbuf + offs, qsign_data.pre_hash, qsign_data.pre_hash_size);
		offs += qsign_data.pre_hash_size;
		memcpy(sbuf + offs, qsign_data.counter, sizeof(qsign_data.counter));
		offs += sizeof(qsign_data.counter);
	}
	else   {
		sbuf[offs++] = 0;
	}

	sbuf[offs++] = 0x80;
	sbuf[offs++] = qsign_data.last_block_size;
	memcpy(sbuf + offs, qsign_data.last_block, qsign_data.last_block_size);
	offs += qsign_data.last_block_size;

	sc_log(ctx,
	       "iasecc_compute_signature_dst() offs %"SC_FORMAT_LEN_SIZE_T"u; OP(meth:%X,ref:%X)",
	       offs, prv->op_method, prv->op_ref);
	if (prv->op_method == SC_AC_SCB && (prv->op_ref & IASECC_SCB_METHOD_SM))
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Not yet");

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x2A, 0x90, 0xA0);
	apdu.data = sbuf;
	apdu.datalen = offs;
	apdu.lc = offs;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "Compute signature failed");

	sc_log(ctx, "iasecc_compute_signature_dst() partial hash OK");

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0x2A, 0x9E, 0x9A);
	apdu.resp = rbuf;
	apdu.resplen = prv->key_size;
	apdu.le = prv->key_size;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "Compute signature failed");

	sc_log(ctx,
	       "iasecc_compute_signature_dst() DST resplen %"SC_FORMAT_LEN_SIZE_T"u",
	       apdu.resplen);
	if (apdu.resplen > out_len)
		LOG_TEST_RET(ctx, SC_ERROR_BUFFER_TOO_SMALL, "Result buffer too small for the DST signature");

	memcpy(out, apdu.resp, apdu.resplen);

	LOG_FUNC_RETURN(ctx, apdu.resplen);
}