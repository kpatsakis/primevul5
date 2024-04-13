static char *cli_dfs_make_full_path(TALLOC_CTX *ctx,
					struct cli_state *cli,
					const char *dir)
{
	char path_sep = '\\';

	/* Ensure the extrapath doesn't start with a separator. */
	while (IS_DIRECTORY_SEP(*dir)) {
		dir++;
	}

	if (cli->requested_posix_capabilities & CIFS_UNIX_POSIX_PATHNAMES_CAP) {
		path_sep = '/';
	}
	return talloc_asprintf(ctx, "%c%s%c%s%c%s",
			path_sep,
			smbXcli_conn_remote_name(cli->conn),
			path_sep,
			cli->share,
			path_sep,
			dir);
}
