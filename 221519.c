
static inline struct io_fixed_file *io_fixed_file_slot(struct io_file_table *table,
						      unsigned i)
{
	struct io_fixed_file *table_l2;

	table_l2 = table->files[i >> IORING_FILE_TABLE_SHIFT];
	return &table_l2[i & IORING_FILE_TABLE_MASK];