lka_proc_forked(const char *name, uint32_t subsystems, int fd)
{
	struct processor_instance	*processor;

	if (!processors_inited) {
		dict_init(&processors);
		processors_inited = 1;
	}

	processor = xcalloc(1, sizeof *processor);
	processor->name = xstrdup(name);
	processor->io = io_new();
	processor->subsystems = subsystems;

	io_set_nonblocking(fd);

	io_set_fd(processor->io, fd);
	io_set_callback(processor->io, processor_io, processor->name);
	dict_xset(&processors, name, processor);
}