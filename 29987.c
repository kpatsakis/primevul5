static struct cli_state *cli_cm_find(struct cli_state *cli,
				const char *server,
				const char *share)
{
	struct cli_state *p;

	if (cli == NULL) {
		return NULL;
	}

	/* Search to the start of the list. */
	for (p = cli; p; p = DLIST_PREV(p)) {
		const char *remote_name =
			smbXcli_conn_remote_name(p->conn);

		if (strequal(server, remote_name) &&
				strequal(share,p->share)) {
			return p;
		}
	}

	/* Search to the end of the list. */
	for (p = cli->next; p; p = p->next) {
		const char *remote_name =
			smbXcli_conn_remote_name(p->conn);

		if (strequal(server, remote_name) &&
				strequal(share,p->share)) {
			return p;
		}
	}

	return NULL;
}
