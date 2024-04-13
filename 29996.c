SMBC_check_server(SMBCCTX * context,
                  SMBCSRV * server)
{
	time_t now;

	if (!cli_state_is_connected(server->cli)) {
		return 1;
	}

	now = time_mono(NULL);

	if (server->last_echo_time == (time_t)0 ||
			now > server->last_echo_time +
				(server->cli->timeout/1000)) {
		unsigned char data[16] = {0};
		NTSTATUS status = cli_echo(server->cli,
					1,
					data_blob_const(data, sizeof(data)));
		if (!NT_STATUS_IS_OK(status)) {
			return 1;
		}
		server->last_echo_time = now;
	}
	return 0;
}
