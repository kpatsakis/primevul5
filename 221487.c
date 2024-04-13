
static bool io_alloc_file_tables(struct io_file_table *table, unsigned nr_files)
{
	unsigned i, nr_tables = DIV_ROUND_UP(nr_files, IORING_MAX_FILES_TABLE);

	table->files = kcalloc(nr_tables, sizeof(*table->files), GFP_KERNEL);
	if (!table->files)
		return false;

	for (i = 0; i < nr_tables; i++) {
		unsigned int this_files = min(nr_files, IORING_MAX_FILES_TABLE);

		table->files[i] = kcalloc(this_files, sizeof(*table->files[i]),
					GFP_KERNEL);
		if (!table->files[i])
			break;
		nr_files -= this_files;
	}

	if (i == nr_tables)
		return true;

	io_free_file_tables(table, nr_tables * IORING_MAX_FILES_TABLE);
	return false;