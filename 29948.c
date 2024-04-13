bool smbXcli_conn_has_async_calls(struct smbXcli_conn *conn)
{
	return ((tevent_queue_length(conn->outgoing) != 0)
		|| (talloc_array_length(conn->pending) != 0));
}
