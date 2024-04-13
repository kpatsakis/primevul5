void smb2cli_conn_set_io_priority(struct smbXcli_conn *conn,
				  uint8_t io_priority)
{
	conn->smb2.io_priority = io_priority;
}
