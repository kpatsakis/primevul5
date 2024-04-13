int git_connection_is_socket(struct child_process *conn)
{
	return conn == &no_fork;
}