void cli_cm_display(struct cli_state *cli)
{
	int i;

	for (i=0; cli; cli = cli->next,i++ ) {
		d_printf("%d:\tserver=%s, share=%s\n",
			i, smbXcli_conn_remote_name(cli->conn), cli->share);
	}
}
