bool smbXcli_conn_use_unicode(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		return true;
	}

	if (conn->smb1.capabilities & CAP_UNICODE) {
		return true;
	}

	return false;
}
