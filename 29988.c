NTSTATUS cli_cm_force_encryption(struct cli_state *c,
			const char *username,
			const char *password,
			const char *domain,
			const char *sharename)
{
	NTSTATUS status;

	if (smbXcli_conn_protocol(c->conn) >= PROTOCOL_SMB2_02) {
		status = smb2cli_session_encryption_on(c->smb2.session);
		if (NT_STATUS_EQUAL(status,NT_STATUS_NOT_SUPPORTED)) {
			d_printf("Encryption required and "
				"server doesn't support "
				"SMB3 encryption - failing connect\n");
		} else if (!NT_STATUS_IS_OK(status)) {
			d_printf("Encryption required and "
				"setup failed with error %s.\n",
				nt_errstr(status));
		}
		return status;
	}

	status = cli_force_encryption(c,
					username,
					password,
					domain);

	if (NT_STATUS_EQUAL(status,NT_STATUS_NOT_SUPPORTED)) {
		d_printf("Encryption required and "
			"server that doesn't support "
			"UNIX extensions - failing connect\n");
	} else if (NT_STATUS_EQUAL(status,NT_STATUS_UNKNOWN_REVISION)) {
		d_printf("Encryption required and "
			"can't get UNIX CIFS extensions "
			"version from server.\n");
	} else if (NT_STATUS_EQUAL(status,NT_STATUS_UNSUPPORTED_COMPRESSION)) {
		d_printf("Encryption required and "
			"share %s doesn't support "
			"encryption.\n", sharename);
	} else if (!NT_STATUS_IS_OK(status)) {
		d_printf("Encryption required and "
			"setup failed with error %s.\n",
			nt_errstr(status));
	}

	return status;
}
