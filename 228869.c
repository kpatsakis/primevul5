filter_session_io(struct io *io, int evt, void *arg)
{
	struct filter_session *fs = arg;
	char *line = NULL;
	ssize_t len;

	log_trace(TRACE_IO, "filter session: %p: %s %s", fs, io_strevent(evt),
	    io_strio(io));

	switch (evt) {
	case IO_DATAIN:
	nextline:
		line = io_getline(fs->io, &len);
		/* No complete line received */
		if (line == NULL)
			return;

		filter_data(fs->id, line);

		goto nextline;
	}
}