iasecc_pin_reset(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	struct sc_context *ctx = card->ctx;
	struct sc_file *save_current = NULL;
	struct iasecc_sdo sdo;
	struct sc_apdu apdu;
	unsigned reference, scb;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "Reset PIN(ref:%i,lengths:%i/%i)", data->pin_reference, data->pin1.len, data->pin2.len);

	if (data->pin_type != SC_AC_CHV)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Unblock procedure can be used only with the PINs of type CHV");

	reference = data->pin_reference;

	if (!(data->pin_reference & IASECC_OBJECT_REF_LOCAL) && card->cache.valid && card->cache.current_df)  {
		struct sc_path path;

		sc_file_dup(&save_current, card->cache.current_df);
		if (save_current == NULL)
			LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot duplicate current DF file");

		sc_format_path("3F00", &path);
		path.type = SC_PATH_TYPE_FILE_ID;
		rv = iasecc_select_file(card, &path, NULL);
		LOG_TEST_RET(ctx, rv, "Unable to select MF");
	}

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_CHV;
	sdo.sdo_ref = reference & ~IASECC_OBJECT_REF_LOCAL;

	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_RET(ctx, rv, "Cannot get PIN data");

	if (sdo.docp.acls_contact.size == 0)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Extremely strange ... there are no ACLs");

	scb = sdo.docp.scbs[IASECC_ACLS_CHV_RESET];
	do   {
		unsigned need_all = scb & IASECC_SCB_METHOD_NEED_ALL ? 1 : 0;
		unsigned char se_num = scb & IASECC_SCB_METHOD_MASK_REF;

		if (scb & IASECC_SCB_METHOD_USER_AUTH)   {
			sc_log(ctx, "Verify PIN in SE %X", se_num);
			rv = iasecc_pin_verify(card, SC_AC_SEN, se_num, data->pin1.data, data->pin1.len, tries_left);
			LOG_TEST_RET(ctx, rv, "iasecc_pin_reset() verify PUK error");

			if (!need_all)
				break;
		}

		if (scb & IASECC_SCB_METHOD_SM)   {
			rv = iasecc_sm_pin_reset(card, se_num, data);
			LOG_FUNC_RETURN(ctx, rv);
		}

		if (scb & IASECC_SCB_METHOD_EXT_AUTH)   {
			rv =  iasecc_sm_external_authentication(card, reference, tries_left);
			LOG_TEST_RET(ctx, rv, "iasecc_pin_reset() external authentication error");
		}
	} while(0);

	iasecc_sdo_free_fields(card, &sdo);

	if (data->pin2.len)   {
		sc_log(ctx, "Reset PIN %X and set new value", reference);
		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x2C, 0x02, reference);
		apdu.data = data->pin2.data;
		apdu.datalen = data->pin2.len;
		apdu.lc = apdu.datalen;

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "PIN cmd failed");
	}
	else if (data->pin2.data) {
		sc_log(ctx, "Reset PIN %X and set new value", reference);
		sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x2C, 3, reference);

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "PIN cmd failed");
	}
	else   {
		sc_log(ctx, "Reset PIN %X and set new value with PIN-PAD", reference);
#if 0
		rv = iasecc_chv_set_pinpad(card, reference);
		LOG_TEST_RET(ctx, rv, "Reset PIN failed");
#else
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Reset retry counter with PIN PAD not supported ");
#endif
	}

	if (save_current)   {
		rv = iasecc_select_file(card, &save_current->path, NULL);
		LOG_TEST_RET(ctx, rv, "Cannot return to saved PATH");
	}

	LOG_FUNC_RETURN(ctx, rv);
}