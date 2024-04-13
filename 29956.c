const struct sockaddr_storage *smbXcli_conn_remote_sockaddr(struct smbXcli_conn *conn)
{
	return &conn->remote_ss;
}
