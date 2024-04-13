iasecc_chv_verify(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd,
		int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_acl_entry acl = pin_cmd->pin1.acls[IASECC_ACLS_CHV_VERIFY];
	struct sc_apdu apdu;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Verify CHV PIN(ref:%i,len:%i,acl:%X:%X)", pin_cmd->pin_reference, pin_cmd->pin1.len,
			acl.method, acl.key_ref);

	if (acl.method & IASECC_SCB_METHOD_SM)   {
		rv = iasecc_sm_pin_verify(card, acl.key_ref, pin_cmd, tries_left);
		LOG_FUNC_RETURN(ctx, rv);
	}

	if (pin_cmd->pin1.data && !pin_cmd->pin1.len)   {
		sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x20, 0, pin_cmd->pin_reference);
	}
	else if (pin_cmd->pin1.data && pin_cmd->pin1.len)   {
		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x20, 0, pin_cmd->pin_reference);
		apdu.data = pin_cmd->pin1.data;
		apdu.datalen = pin_cmd->pin1.len;
		apdu.lc = pin_cmd->pin1.len;
	}
	else if ((card->reader->capabilities & SC_READER_CAP_PIN_PAD) && !pin_cmd->pin1.data && !pin_cmd->pin1.len)   {
		rv = iasecc_chv_verify_pinpad(card, pin_cmd, tries_left);
		sc_log(ctx, "Result of verifying CHV with PIN pad %i", rv);
		LOG_FUNC_RETURN(ctx, rv);
	}
	else   {
		LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);
	}

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");

	if (tries_left && apdu.sw1 == 0x63 && (apdu.sw2 & 0xF0) == 0xC0)
		*tries_left = apdu.sw2 & 0x0F;

	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);

	LOG_FUNC_RETURN(ctx, rv);
}