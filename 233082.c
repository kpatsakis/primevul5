iasecc_select_mf(struct sc_card *card, struct sc_file **file_out)
{
	struct sc_context *ctx = card->ctx;
	struct sc_file *mf_file = NULL;
	struct sc_path path;
	int rv;

	LOG_FUNC_CALLED(ctx);

	if (file_out)
		*file_out = NULL;

	memset(&path, 0, sizeof(struct sc_path));
	if (!card->ef_atr || !card->ef_atr->aid.len)   {
		struct sc_apdu apdu;
		unsigned char apdu_resp[SC_MAX_APDU_BUFFER_SIZE];

		/* ISO 'select' command fails when not FCP data returned */
		sc_format_path("3F00", &path);
		path.type = SC_PATH_TYPE_FILE_ID;

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xA4, 0x00, 0x00);
		apdu.lc = path.len;
		apdu.data = path.value;
		apdu.datalen = path.len;
		apdu.resplen = sizeof(apdu_resp);
		apdu.resp = apdu_resp;

		if (card->type == SC_CARD_TYPE_IASECC_MI2)
			apdu.p2 = 0x04;

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(card->ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(card->ctx, rv, "Cannot select MF");
	}
	else   {
		memset(&path, 0, sizeof(path));
		path.type = SC_PATH_TYPE_DF_NAME;
		memcpy(path.value, card->ef_atr->aid.value, card->ef_atr->aid.len);
		path.len = card->ef_atr->aid.len;
		rv = iasecc_select_file(card, &path, file_out);
		LOG_TEST_RET(ctx, rv, "Unable to ROOT selection");
	}

	/* Ignore the FCP of the MF, because:
	 * - some cards do not return it;
	 * - there is not need of it -- create/delete of the files in MF is not envisaged.
	 */
	mf_file = sc_file_new();
	if (mf_file == NULL)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot allocate MF file");
	mf_file->type = SC_FILE_TYPE_DF;
	mf_file->path = path;

	if (card->cache.valid)
		 sc_file_free(card->cache.current_df);
	card->cache.current_df = NULL;

	if (card->cache.valid)
		sc_file_free(card->cache.current_ef);
	card->cache.current_ef = NULL;

	sc_file_dup(&card->cache.current_df, mf_file);
	card->cache.valid = 1;

	if (file_out && *file_out == NULL)
		*file_out = mf_file;
	else
		sc_file_free(mf_file);

	LOG_FUNC_RETURN(ctx, rv);
}