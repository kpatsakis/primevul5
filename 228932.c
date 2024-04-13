lka_proc_errfd(const char *name, int fd)
{
	struct processor_instance	*processor;

	processor = dict_xget(&processors, name);

	io_set_nonblocking(fd);

	processor->errfd = io_new();
	io_set_fd(processor->errfd, fd);
	io_set_callback(processor->errfd, processor_errfd, processor->name);

	lka_proc_config(processor);
}