iasecc_pin_change(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned reference = data->pin_reference;
	unsigned char pin_data[0x100];
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Change PIN(ref:%i,type:0x%X,lengths:%i/%i)", reference, data->pin_type, data->pin1.len, data->pin2.len);

	if ((card->reader->capabilities & SC_READER_CAP_PIN_PAD))   {
		if (!data->pin1.data && !data->pin1.len && !data->pin2.data && !data->pin2.len)   {
			rv = iasecc_chv_change_pinpad(card, reference, tries_left);
			sc_log(ctx, "iasecc_pin_cmd(SC_PIN_CMD_CHANGE) chv_change_pinpad returned %i", rv);
			LOG_FUNC_RETURN(ctx, rv);
		}
	}

	if (!data->pin1.data && data->pin1.len)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Invalid PIN1 arguments");

	if (!data->pin2.data && data->pin2.len)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Invalid PIN2 arguments");

	rv = iasecc_pin_verify(card, data->pin_type, reference, data->pin1.data, data->pin1.len, tries_left);
	sc_log(ctx, "iasecc_pin_cmd(SC_PIN_CMD_CHANGE) pin_verify returned %i", rv);
	LOG_TEST_RET(ctx, rv, "PIN verification error");

	if ((unsigned)(data->pin1.len + data->pin2.len) > sizeof(pin_data))
		LOG_TEST_RET(ctx, SC_ERROR_BUFFER_TOO_SMALL, "Buffer too small for the 'Change PIN' data");

	if (data->pin1.data)
		memcpy(pin_data, data->pin1.data, data->pin1.len);
	if (data->pin2.data)
		memcpy(pin_data + data->pin1.len, data->pin2.data, data->pin2.len);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x24, 0, reference);
	apdu.data = pin_data;
	apdu.datalen = data->pin1.len + data->pin2.len;
	apdu.lc = apdu.datalen;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "PIN cmd failed");

	LOG_FUNC_RETURN(ctx, rv);
}