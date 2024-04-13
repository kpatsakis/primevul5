cleanup_exit(int i)
{
	cleanup_socket();
	_exit(i);
}