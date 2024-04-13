static void cli_set_mntpoint(struct cli_state *cli, const char *mnt)
{
	TALLOC_CTX *frame = talloc_stackframe();
	char *name = clean_name(frame, mnt);
	if (!name) {
		TALLOC_FREE(frame);
		return;
	}
	TALLOC_FREE(cli->dfs_mountpoint);
	cli->dfs_mountpoint = talloc_strdup(cli, name);
	TALLOC_FREE(frame);
}
