
static void io_free_file_tables(struct io_file_table *table, unsigned nr_files)
{
	unsigned i, nr_tables = DIV_ROUND_UP(nr_files, IORING_MAX_FILES_TABLE);

	for (i = 0; i < nr_tables; i++)
		kfree(table->files[i]);
	kfree(table->files);
	table->files = NULL;