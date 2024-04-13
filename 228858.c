processor_errfd(struct io *io, int evt, void *arg)
{
	const char	*name = arg;
	char		*line = NULL;
	ssize_t		 len;

	switch (evt) {
	case IO_DATAIN:
		while ((line = io_getline(io, &len)) != NULL)
			log_warnx("%s: %s", name, line);
	}
}