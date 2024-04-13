iasecc_pin_is_verified(struct sc_card *card, struct sc_pin_cmd_data *pin_cmd_data,
		int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_pin_cmd_data pin_cmd;
        struct sc_acl_entry acl = pin_cmd_data->pin1.acls[IASECC_ACLS_CHV_VERIFY];
	int rv = SC_ERROR_SECURITY_STATUS_NOT_SATISFIED;

	LOG_FUNC_CALLED(ctx);

	if (pin_cmd_data->pin_type != SC_AC_CHV)
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "PIN type is not supported for the verification");

	sc_log(ctx, "Verify ACL(method:%X;ref:%X)", acl.method, acl.key_ref);
	if (acl.method != IASECC_SCB_ALWAYS)
		LOG_FUNC_RETURN(ctx, SC_ERROR_SECURITY_STATUS_NOT_SATISFIED);

	pin_cmd = *pin_cmd_data;
	pin_cmd.pin1.data = (unsigned char *)"";
	pin_cmd.pin1.len = 0;

	rv = iasecc_chv_verify(card, &pin_cmd, tries_left);

	LOG_FUNC_RETURN(ctx, rv);
}