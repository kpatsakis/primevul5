processor_io(struct io *io, int evt, void *arg)
{
	struct processor_instance *processor;
	const char		*name = arg;
	char			*line = NULL;
	ssize_t			 len;

	switch (evt) {
	case IO_DATAIN:
		while ((line = io_getline(io, &len)) != NULL) {
			if (strncmp("register|", line, 9) == 0) {
				processor_register(name, line);
				continue;
			}
			
			processor = dict_xget(&processors, name);
			if (!processor->ready)
				fatalx("Non-register message before register|"
				    "ready: %s", line);
			else if (strncmp(line, "filter-result|", 14) == 0 ||
			    strncmp(line, "filter-dataline|", 16) == 0)
				lka_filter_process_response(name, line);
			else if (strncmp(line, "report|", 7) == 0)
				lka_report_proc(name, line);
			else
				fatalx("Invalid filter message type: %s", line);
		}
	}
}