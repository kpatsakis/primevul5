void smbXcli_conn_set_sockopt(struct smbXcli_conn *conn, const char *options)
{
	set_socket_options(conn->sock_fd, options);
}
