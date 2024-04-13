const struct sockaddr_storage *smbXcli_conn_local_sockaddr(struct smbXcli_conn *conn)
{
	return &conn->local_ss;
}
