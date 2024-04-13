iasecc_pin_get_policy (struct sc_card *card, struct sc_pin_cmd_data *data)
{
	struct sc_context *ctx = card->ctx;
	struct sc_file *save_current_df = NULL, *save_current_ef = NULL;
	struct iasecc_sdo sdo;
	struct sc_path path;
	unsigned ii;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_pin_get_policy(card:%p)", card);

	if (data->pin_type != SC_AC_CHV)   {
		sc_log(ctx, "To unblock PIN it's CHV reference should be presented");
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);
	}

	if (card->cache.valid && card->cache.current_df)   {
		sc_file_dup(&save_current_df, card->cache.current_df);
		if (save_current_df == NULL) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			sc_log(ctx, "Cannot duplicate current DF file");
			goto err;
		}
	}

	if (card->cache.valid && card->cache.current_ef)   {
		sc_file_dup(&save_current_ef, card->cache.current_ef);
		if (save_current_ef == NULL) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			sc_log(ctx, "Cannot duplicate current EF file");
			goto err;
		}
	}

	if (!(data->pin_reference & IASECC_OBJECT_REF_LOCAL) && card->cache.valid && card->cache.current_df) {
		sc_format_path("3F00", &path);
		path.type = SC_PATH_TYPE_FILE_ID;
		rv = iasecc_select_file(card, &path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Unable to select MF");
	}

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_CHV;

	sdo.sdo_ref = data->pin_reference & ~IASECC_OBJECT_REF_LOCAL;

	sc_log(ctx, "iasecc_pin_get_policy() reference %i", sdo.sdo_ref);

	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_GOTO_ERR(ctx, rv, "Cannot get SDO PIN data");

	if (sdo.docp.acls_contact.size == 0) {
		rv = SC_ERROR_INVALID_DATA;
		sc_log(ctx, "Extremely strange ... there is no ACLs");
		goto err;
	}

	sc_log(ctx,
	       "iasecc_pin_get_policy() sdo.docp.size.size %"SC_FORMAT_LEN_SIZE_T"u",
	       sdo.docp.size.size);
	for (ii=0; ii<sizeof(sdo.docp.scbs); ii++)   {
		struct iasecc_se_info se;
		unsigned char scb = sdo.docp.scbs[ii];
		struct sc_acl_entry *acl = &data->pin1.acls[ii];
		int crt_num = 0;

		memset(&se, 0, sizeof(se));
		memset(&acl->crts, 0, sizeof(acl->crts));

		sc_log(ctx, "iasecc_pin_get_policy() set info acls: SCB 0x%X", scb);
		/* acl->raw_value = scb; */
		acl->method = scb & IASECC_SCB_METHOD_MASK;
		acl->key_ref = scb & IASECC_SCB_METHOD_MASK_REF;

		if (scb==0 || scb==0xFF)
			continue;

		if (se.reference != (int)acl->key_ref)   {
			memset(&se, 0, sizeof(se));

			se.reference = acl->key_ref;

			rv = iasecc_se_get_info(card, &se);
			LOG_TEST_GOTO_ERR(ctx, rv, "SDO get data error");
		}

		if (scb & IASECC_SCB_METHOD_USER_AUTH)   {
			rv = iasecc_se_get_crt_by_usage(card, &se,
					IASECC_CRT_TAG_AT, IASECC_UQB_AT_USER_PASSWORD, &acl->crts[crt_num]);
			LOG_TEST_GOTO_ERR(ctx, rv, "no authentication template for 'USER PASSWORD'");
			sc_log(ctx, "iasecc_pin_get_policy() scb:0x%X; sdo_ref:[%i,%i,...]",
					scb, acl->crts[crt_num].refs[0], acl->crts[crt_num].refs[1]);
			crt_num++;
		}

		if (scb & (IASECC_SCB_METHOD_SM | IASECC_SCB_METHOD_EXT_AUTH))   {
			sc_log(ctx, "'SM' and 'EXTERNAL AUTHENTICATION' protection methods are not supported: SCB:0x%X", scb);
			/* Set to 'NEVER' if all conditions are needed or
			 * there is no user authentication method allowed */
			if (!crt_num || (scb & IASECC_SCB_METHOD_NEED_ALL))
				acl->method = SC_AC_NEVER;
			continue;
		}

		sc_file_free(se.df);
	}

	if (sdo.data.chv.size_max.value)
		data->pin1.max_length = *sdo.data.chv.size_max.value;
	if (sdo.data.chv.size_min.value)
		data->pin1.min_length = *sdo.data.chv.size_min.value;
	if (sdo.docp.tries_maximum.value)
		data->pin1.max_tries = *sdo.docp.tries_maximum.value;
	if (sdo.docp.tries_remaining.value)
		data->pin1.tries_left = *sdo.docp.tries_remaining.value;
	if (sdo.docp.size.value)   {
		for (ii=0; ii<sdo.docp.size.size; ii++)
			data->pin1.stored_length = ((data->pin1.stored_length) << 8) + *(sdo.docp.size.value + ii);
	}

	data->pin1.encoding = SC_PIN_ENCODING_ASCII;
	data->pin1.offset = 5;
	data->pin1.logged_in = SC_PIN_STATE_UNKNOWN;

	sc_log(ctx,
	       "PIN policy: size max/min %"SC_FORMAT_LEN_SIZE_T"u/%"SC_FORMAT_LEN_SIZE_T"u, tries max/left %i/%i",
	       data->pin1.max_length, data->pin1.min_length,
	       data->pin1.max_tries, data->pin1.tries_left);
	iasecc_sdo_free_fields(card, &sdo);

	if (save_current_df)   {
		sc_log(ctx, "iasecc_pin_get_policy() restore current DF");
		rv = iasecc_select_file(card, &save_current_df->path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Cannot return to saved DF");
	}

	if (save_current_ef)   {
		sc_log(ctx, "iasecc_pin_get_policy() restore current EF");
		rv = iasecc_select_file(card, &save_current_ef->path, NULL);
		LOG_TEST_GOTO_ERR(ctx, rv, "Cannot return to saved EF");
	}

err:
	sc_file_free(save_current_df);
	sc_file_free(save_current_ef);

	LOG_FUNC_RETURN(ctx, rv);
}