bool cli_check_msdfs_proxy(TALLOC_CTX *ctx,
				struct cli_state *cli,
				const char *sharename,
				char **pp_newserver,
				char **pp_newshare,
				bool force_encrypt,
				const char *username,
				const char *password,
				const char *domain)
{
	struct client_dfs_referral *refs = NULL;
	size_t num_refs = 0;
	size_t consumed = 0;
	char *fullpath = NULL;
	bool res;
	uint16_t cnum;
	char *newextrapath = NULL;
	NTSTATUS status;
	const char *remote_name;

	if (!cli || !sharename) {
		return false;
	}

	remote_name = smbXcli_conn_remote_name(cli->conn);
	cnum = cli_state_get_tid(cli);

	/* special case.  never check for a referral on the IPC$ share */

	if (strequal(sharename, "IPC$")) {
		return false;
	}

	/* send a trans2_query_path_info to check for a referral */

	fullpath = talloc_asprintf(ctx, "\\%s\\%s", remote_name, sharename);
	if (!fullpath) {
		return false;
	}

	/* check for the referral */

	if (!NT_STATUS_IS_OK(cli_tree_connect(cli, "IPC$", "IPC", NULL, 0))) {
		return false;
	}

	if (force_encrypt) {
		status = cli_cm_force_encryption(cli,
					username,
					password,
					domain,
					"IPC$");
		if (!NT_STATUS_IS_OK(status)) {
			return false;
		}
	}

	status = cli_dfs_get_referral(ctx, cli, fullpath, &refs,
				      &num_refs, &consumed);
	res = NT_STATUS_IS_OK(status);

	status = cli_tdis(cli);
	if (!NT_STATUS_IS_OK(status)) {
		return false;
	}

	cli_state_set_tid(cli, cnum);

	if (!res || !num_refs) {
		return false;
	}

	if (!refs[0].dfspath) {
		return false;
	}

	if (!split_dfs_path(ctx, refs[0].dfspath, pp_newserver,
			    pp_newshare, &newextrapath)) {
		return false;
	}

	/* check that this is not a self-referral */

	if (strequal(remote_name, *pp_newserver) &&
			strequal(sharename, *pp_newshare)) {
		return false;
	}

	return true;
}
