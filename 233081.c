iasecc_init_oberthur(struct sc_card *card)
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

	iasecc_parse_ef_atr(card);

	/* if we fail to select CM, */
	if (gp_select_card_manager(card)) {
		gp_select_isd_rid(card);
	}

	rv = iasecc_oberthur_match(card);
	LOG_TEST_RET(ctx, rv, "unknown Oberthur's IAS/ECC card");

	rv = iasecc_select_mf(card, NULL);
	LOG_TEST_RET(ctx, rv, "MF selection error");

	rv = iasecc_parse_ef_atr(card);
	LOG_TEST_RET(ctx, rv, "EF.ATR read or parse error");

	sc_log(ctx, "EF.ATR(aid:'%s')", sc_dump_hex(card->ef_atr->aid.value, card->ef_atr->aid.len));
	LOG_FUNC_RETURN(ctx, rv);
}