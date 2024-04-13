static int samldb_check_user_account_control_invariants(struct samldb_ctx *ac,
						    uint32_t user_account_control)
{
	int i, ret = 0;
	bool need_check = false;
	const struct uac_to_guid {
		uint32_t uac;
		bool never;
		uint32_t needs;
		uint32_t not_with;
		const char *error_string;
	} map[] = {
		{
			.uac = UF_TEMP_DUPLICATE_ACCOUNT,
			.never = true,
			.error_string = "Updating the UF_TEMP_DUPLICATE_ACCOUNT flag is never allowed"
		},
		{
			.uac = UF_PARTIAL_SECRETS_ACCOUNT,
			.needs = UF_WORKSTATION_TRUST_ACCOUNT,
			.error_string = "Setting UF_PARTIAL_SECRETS_ACCOUNT only permitted with UF_WORKSTATION_TRUST_ACCOUNT"
		},
		{
			.uac = UF_TRUSTED_FOR_DELEGATION,
			.not_with = UF_PARTIAL_SECRETS_ACCOUNT,
			.error_string = "Setting UF_TRUSTED_FOR_DELEGATION not allowed with UF_PARTIAL_SECRETS_ACCOUNT"
		},
		{
			.uac = UF_NORMAL_ACCOUNT,
			.not_with = UF_ACCOUNT_TYPE_MASK & ~UF_NORMAL_ACCOUNT,
			.error_string = "Setting more than one account type not permitted"
		},
		{
			.uac = UF_WORKSTATION_TRUST_ACCOUNT,
			.not_with = UF_ACCOUNT_TYPE_MASK & ~UF_WORKSTATION_TRUST_ACCOUNT,
			.error_string = "Setting more than one account type not permitted"
		},
		{
			.uac = UF_INTERDOMAIN_TRUST_ACCOUNT,
			.not_with = UF_ACCOUNT_TYPE_MASK & ~UF_INTERDOMAIN_TRUST_ACCOUNT,
			.error_string = "Setting more than one account type not permitted"
		},
		{
			.uac = UF_SERVER_TRUST_ACCOUNT,
			.not_with = UF_ACCOUNT_TYPE_MASK & ~UF_SERVER_TRUST_ACCOUNT,
			.error_string = "Setting more than one account type not permitted"
		},
		{
			.uac = UF_TRUSTED_FOR_DELEGATION,
			.not_with = UF_PARTIAL_SECRETS_ACCOUNT,
			.error_string = "Setting UF_TRUSTED_FOR_DELEGATION not allowed with UF_PARTIAL_SECRETS_ACCOUNT"
		}
	};

	for (i = 0; i < ARRAY_SIZE(map); i++) {
		if (user_account_control & map[i].uac) {
			need_check = true;
			break;
		}
	}
	if (need_check == false) {
		return LDB_SUCCESS;
	}

	for (i = 0; i < ARRAY_SIZE(map); i++) {
		uint32_t this_uac = user_account_control & map[i].uac;
		if (this_uac != 0) {
			if (map[i].never) {
				ret = LDB_ERR_OTHER;
				break;
			} else if (map[i].needs != 0) {
				if ((map[i].needs & user_account_control) == 0) {
					ret = LDB_ERR_OTHER;
					break;
				}
			} else if (map[i].not_with != 0) {
				if ((map[i].not_with & user_account_control) != 0) {
					ret = LDB_ERR_OTHER;
					break;
				}
			}
		}
	}
	if (ret != LDB_SUCCESS) {
		switch (ac->req->operation) {
		case LDB_ADD:
			ldb_asprintf_errstring(ldb_module_get_ctx(ac->module),
					       "Failed to add %s: %s",
					       ldb_dn_get_linearized(ac->msg->dn),
					       map[i].error_string);
			break;
		case LDB_MODIFY:
			ldb_asprintf_errstring(ldb_module_get_ctx(ac->module),
					       "Failed to modify %s: %s",
					       ldb_dn_get_linearized(ac->msg->dn),
					       map[i].error_string);
			break;
		default:
			return ldb_module_operr(ac->module);
		}
	}
	return ret;
}
