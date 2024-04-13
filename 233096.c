iasecc_init(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_private_data *private_data = NULL;
	int rv = SC_ERROR_NO_CARD_SUPPORT;

	LOG_FUNC_CALLED(ctx);
	private_data = (struct iasecc_private_data *) calloc(1, sizeof(struct iasecc_private_data));
	if (private_data == NULL)
		LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);

	card->cla  = 0x00;
	card->drv_data = private_data;

	if (card->type == SC_CARD_TYPE_IASECC_GEMALTO)
		rv = iasecc_init_gemalto(card);
	else if (card->type == SC_CARD_TYPE_IASECC_OBERTHUR)
		rv = iasecc_init_oberthur(card);
	else if (card->type == SC_CARD_TYPE_IASECC_SAGEM)
		rv = iasecc_init_amos_or_sagem(card);
	else if (card->type == SC_CARD_TYPE_IASECC_AMOS)
		rv = iasecc_init_amos_or_sagem(card);
	else if (card->type == SC_CARD_TYPE_IASECC_MI)
		rv = iasecc_init_amos_or_sagem(card);
	else
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_CARD);


	if (!rv)   {
		if (card->ef_atr && card->ef_atr->aid.len)   {
			struct sc_path path;

			memset(&path, 0, sizeof(struct sc_path));
			path.type = SC_PATH_TYPE_DF_NAME;
			memcpy(path.value, card->ef_atr->aid.value, card->ef_atr->aid.len);
			path.len = card->ef_atr->aid.len;

			rv = iasecc_select_file(card, &path, NULL);
			sc_log(ctx, "Select ECC ROOT with the AID from EF.ATR: rv %i", rv);
			LOG_TEST_RET(ctx, rv, "Select EF.ATR AID failed");
		}

		rv = iasecc_get_serialnr(card, NULL);
	}

#ifdef ENABLE_SM
	card->sm_ctx.ops.read_binary = _iasecc_sm_read_binary;
	card->sm_ctx.ops.update_binary = _iasecc_sm_update_binary;
#endif

	if (!rv) {
		sc_log(ctx, "EF.ATR(aid:'%s')", sc_dump_hex(card->ef_atr->aid.value, card->ef_atr->aid.len));
		rv = SC_ERROR_INVALID_CARD;
	}
	LOG_FUNC_RETURN(ctx, rv);
}