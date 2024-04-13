st_memsize(const st_table *table)
{
    if (table->entries_packed) {
	return table->num_bins * sizeof (void *) + sizeof(st_table);
    }
    else {
	return table->num_entries * sizeof(struct st_table_entry) + table->num_bins * sizeof (void *) + sizeof(st_table);
    }
}