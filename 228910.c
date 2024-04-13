processor_register(const char *name, const char *line)
{
	struct processor_instance *processor;

	processor = dict_xget(&processors, name);

	if (strcmp(line, "register|ready") == 0) {
		processor->ready = 1;
		return;
	}

	if (strncmp(line, "register|report|", 16) == 0) {
		lka_report_register_hook(name, line+16);
		return;
	}

	if (strncmp(line, "register|filter|", 16) == 0) {
		lka_filter_register_hook(name, line+16);
		return;
	}

	fatalx("Invalid register line received: %s", line);
}