iasecc_get_free_reference(struct sc_card *card, struct iasecc_ctl_get_free_reference *ctl_data)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo *sdo = NULL;
	int idx, rv;

	LOG_FUNC_CALLED(ctx);

	if ((ctl_data->key_size % 0x40) || ctl_data->index < 1 || (ctl_data->index > IASECC_OBJECT_REF_MAX))
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	sc_log(ctx, "get reference for key(index:%i,usage:%X,access:%X)", ctl_data->index, ctl_data->usage, ctl_data->access);
	/* TODO: when looking for the slot for the signature keys, check also PSO_SIGNATURE ACL */
	for (idx = ctl_data->index; idx <= IASECC_OBJECT_REF_MAX; idx++)   {
		unsigned char sdo_tag[3] = {
			IASECC_SDO_TAG_HEADER, IASECC_OBJECT_REF_LOCAL | IASECC_SDO_CLASS_RSA_PRIVATE, idx
		};
		size_t sz;

		if (sdo)
			iasecc_sdo_free(card, sdo);

		rv = iasecc_sdo_allocate_and_parse(card, sdo_tag, 3, &sdo);
		LOG_TEST_RET(ctx, rv, "cannot parse SDO data");

		rv = iasecc_sdo_get_data(card, sdo);
		if (rv == SC_ERROR_DATA_OBJECT_NOT_FOUND)   {
			iasecc_sdo_free(card, sdo);

			sc_log(ctx, "found empty key slot %i", idx);
			break;
		}
		else
			LOG_TEST_RET(ctx, rv, "get new key reference failed");

		sz = *(sdo->docp.size.value + 0) * 0x100 + *(sdo->docp.size.value + 1);
		sc_log(ctx,
		       "SDO(idx:%i) size %"SC_FORMAT_LEN_SIZE_T"u; key_size %"SC_FORMAT_LEN_SIZE_T"u",
		       idx, sz, ctl_data->key_size);

		if (sz != ctl_data->key_size / 8)   {
			sc_log(ctx,
			       "key index %i ignored: different key sizes %"SC_FORMAT_LEN_SIZE_T"u/%"SC_FORMAT_LEN_SIZE_T"u",
			       idx, sz, ctl_data->key_size / 8);
			continue;
		}

		if (sdo->docp.non_repudiation.value)   {
			sc_log(ctx, "non repudiation flag %X", sdo->docp.non_repudiation.value[0]);
			if ((ctl_data->usage & SC_PKCS15_PRKEY_USAGE_NONREPUDIATION) && !(*sdo->docp.non_repudiation.value))   {
				sc_log(ctx, "key index %i ignored: need non repudiation", idx);
				continue;
			}

			if (!(ctl_data->usage & SC_PKCS15_PRKEY_USAGE_NONREPUDIATION) && *sdo->docp.non_repudiation.value)   {
				sc_log(ctx, "key index %i ignored: don't need non-repudiation", idx);
				continue;
			}
		}

		if (ctl_data->access & SC_PKCS15_PRKEY_ACCESS_LOCAL)   {
			if (sdo->docp.scbs[IASECC_ACLS_RSAKEY_GENERATE] == IASECC_SCB_NEVER)   {
				sc_log(ctx, "key index %i ignored: GENERATE KEY not allowed", idx);
				continue;
			}
		}
		else   {
			if (sdo->docp.scbs[IASECC_ACLS_RSAKEY_PUT_DATA] == IASECC_SCB_NEVER)   {
				sc_log(ctx, "key index %i ignored: PUT DATA not allowed", idx);
				continue;
			}
		}

		if ((ctl_data->usage & SC_PKCS15_PRKEY_USAGE_NONREPUDIATION) && (ctl_data->usage & SC_PKCS15_PRKEY_USAGE_SIGN))   {
			if (sdo->docp.scbs[IASECC_ACLS_RSAKEY_PSO_SIGN] == IASECC_SCB_NEVER)   {
				sc_log(ctx, "key index %i ignored: PSO SIGN not allowed", idx);
				continue;
			}
		}
		else if (ctl_data->usage & SC_PKCS15_PRKEY_USAGE_SIGN)   {
			if (sdo->docp.scbs[IASECC_ACLS_RSAKEY_INTERNAL_AUTH] == IASECC_SCB_NEVER)   {
				sc_log(ctx, "key index %i ignored: INTERNAL AUTHENTICATE not allowed", idx);
				continue;
			}
		}

		if (ctl_data->usage & (SC_PKCS15_PRKEY_USAGE_DECRYPT | SC_PKCS15_PRKEY_USAGE_UNWRAP))   {
			if (sdo->docp.scbs[IASECC_ACLS_RSAKEY_PSO_DECIPHER] == IASECC_SCB_NEVER)   {
				sc_log(ctx, "key index %i ignored: PSO DECIPHER not allowed", idx);
				continue;
			}
		}

		break;
	}

	ctl_data->index = idx;

	if (idx > IASECC_OBJECT_REF_MAX)
		LOG_FUNC_RETURN(ctx, SC_ERROR_DATA_OBJECT_NOT_FOUND);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}