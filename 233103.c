iasecc_pin_cmd(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_pin_cmd() cmd 0x%X, PIN type 0x%X, PIN reference %i, PIN-1 %p:%i, PIN-2 %p:%i",
			data->cmd, data->pin_type, data->pin_reference,
			data->pin1.data, data->pin1.len, data->pin2.data, data->pin2.len);

	switch (data->cmd)   {
	case SC_PIN_CMD_VERIFY:
		rv = iasecc_pin_verify(card, data->pin_type, data->pin_reference, data->pin1.data, data->pin1.len, tries_left);
		break;
	case SC_PIN_CMD_CHANGE:
		if (data->pin_type == SC_AC_AUT)
			rv = iasecc_keyset_change(card, data, tries_left);
		else
			rv = iasecc_pin_change(card, data, tries_left);
		break;
	case SC_PIN_CMD_UNBLOCK:
		rv = iasecc_pin_reset(card, data, tries_left);
		break;
	case SC_PIN_CMD_GET_INFO:
		rv = iasecc_pin_get_policy(card, data);
		break;
	default:
		sc_log(ctx, "Other pin commands not supported yet: 0x%X", data->cmd);
		rv = SC_ERROR_NOT_SUPPORTED;
	}

	LOG_FUNC_RETURN(ctx, rv);
}