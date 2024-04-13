st_clear(st_table *table)
{
    register st_table_entry *ptr, *next;
    st_index_t i;

    if (table->entries_packed) {
        table->num_entries = 0;
        return;
    }

    for(i = 0; i < table->num_bins; i++) {
	ptr = table->bins[i];
	table->bins[i] = 0;
	while (ptr != 0) {
	    next = ptr->next;
	    free(ptr);
	    ptr = next;
	}
    }
    table->num_entries = 0;
    table->head = 0;
    table->tail = 0;
}