iasecc_init_amos_or_sagem(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	unsigned int flags;
	int rv = 0;

	LOG_FUNC_CALLED(ctx);

	flags = IASECC_CARD_DEFAULT_FLAGS;

	_sc_card_add_rsa_alg(card, 1024, flags, 0x10001);
	_sc_card_add_rsa_alg(card, 2048, flags, 0x10001);

	card->caps = SC_CARD_CAP_RNG;
	card->caps |= SC_CARD_CAP_APDU_EXT;
	card->caps |= SC_CARD_CAP_USE_FCI_AC;

	if (card->type == SC_CARD_TYPE_IASECC_MI)   {
		rv = iasecc_mi_match(card);
		if (rv)
			card->type = SC_CARD_TYPE_IASECC_MI2;
		else
			LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	}

	rv = iasecc_parse_ef_atr(card);
	if (rv == SC_ERROR_FILE_NOT_FOUND)   {
		rv = iasecc_select_mf(card, NULL);
		LOG_TEST_RET(ctx, rv, "MF selection error");

		rv = iasecc_parse_ef_atr(card);
	}
	LOG_TEST_RET(ctx, rv, "IASECC: ATR parse failed");

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}