lka_proc_get_io(const char *name)
{
	struct processor_instance *processor;

	processor = dict_xget(&processors, name);

	return processor->io;
}