static NTSTATUS cli_cm_connect(TALLOC_CTX *ctx,
			       struct cli_state *referring_cli,
			       const char *server,
			       const char *share,
			       const struct user_auth_info *auth_info,
			       bool show_hdr,
			       bool force_encrypt,
			       int max_protocol,
			       int port,
			       int name_type,
			       struct cli_state **pcli)
{
	struct cli_state *cli;
	NTSTATUS status;

	status = do_connect(ctx, server, share,
				auth_info,
				show_hdr, force_encrypt, max_protocol,
				port, name_type, &cli);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* Enter into the list. */
	if (referring_cli) {
		DLIST_ADD_END(referring_cli, cli, struct cli_state *);
	}

	if (referring_cli && referring_cli->requested_posix_capabilities) {
		uint16_t major, minor;
		uint32_t caplow, caphigh;
		status = cli_unix_extensions_version(cli, &major, &minor,
						     &caplow, &caphigh);
		if (NT_STATUS_IS_OK(status)) {
			cli_set_unix_extensions_capabilities(cli,
					major, minor,
					caplow, caphigh);
		}
	}

	*pcli = cli;
	return NT_STATUS_OK;
}
