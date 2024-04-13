iasecc_pin_verify(struct sc_card *card, unsigned type, unsigned reference,
		const unsigned char *data, size_t data_len, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_pin_cmd_data pin_cmd;
	unsigned chv_ref = reference;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "Verify PIN(type:%X,ref:%i,data(len:%"SC_FORMAT_LEN_SIZE_T"u,%p)",
	       type, reference, data_len, data);

	if (type == SC_AC_AUT)   {
		rv =  iasecc_sm_external_authentication(card, reference, tries_left);
		LOG_FUNC_RETURN(ctx, rv);
	}
	else if (type == SC_AC_SCB)   {
		if (reference & IASECC_SCB_METHOD_USER_AUTH)   {
			type = SC_AC_SEN;
			reference = reference & IASECC_SCB_METHOD_MASK_REF;
		}
		else   {
			sc_log(ctx, "Do not try to verify non CHV PINs");
			LOG_FUNC_RETURN(ctx, SC_SUCCESS);
		}
	}

	if (type == SC_AC_SEN)   {
		rv = iasecc_se_at_to_chv_reference(card, reference,  &chv_ref);
		LOG_TEST_RET(ctx, rv, "SE AT to CHV reference error");
	}

	memset(&pin_cmd, 0, sizeof(pin_cmd));
	pin_cmd.pin_type = SC_AC_CHV;
	pin_cmd.pin_reference = chv_ref;
	pin_cmd.cmd = SC_PIN_CMD_VERIFY;

	rv = iasecc_pin_get_policy(card, &pin_cmd);
	LOG_TEST_RET(ctx, rv, "Get 'PIN policy' error");

	pin_cmd.pin1.data = data;
	pin_cmd.pin1.len = data_len;

	rv = iasecc_pin_is_verified(card, &pin_cmd, tries_left);
	if (data && !data_len)
		LOG_FUNC_RETURN(ctx, rv);

	if (!rv)   {
		if (iasecc_chv_cache_is_verified(card, &pin_cmd))
			LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	}
	else if (rv != SC_ERROR_PIN_CODE_INCORRECT && rv != SC_ERROR_SECURITY_STATUS_NOT_SATISFIED)   {
		LOG_FUNC_RETURN(ctx, rv);
	}

	iasecc_chv_cache_clean(card, &pin_cmd);

	rv = iasecc_chv_verify(card, &pin_cmd, tries_left);
	LOG_TEST_RET(ctx, rv, "PIN CHV verification error");

	rv = iasecc_chv_cache_verified(card, &pin_cmd);

	LOG_FUNC_RETURN(ctx, rv);
}