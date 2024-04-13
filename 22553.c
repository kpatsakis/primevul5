st_copy(st_table *old_table)
{
    st_table *new_table;
    st_table_entry *ptr, *entry, *prev, **tail;
    st_index_t num_bins = old_table->num_bins;
    st_index_t hash_val;

    new_table = alloc(st_table);
    if (new_table == 0) {
	return 0;
    }

    *new_table = *old_table;
    new_table->bins = (st_table_entry**)
	Calloc((unsigned)num_bins, sizeof(st_table_entry*));

    if (new_table->bins == 0) {
	free(new_table);
	return 0;
    }

    if (old_table->entries_packed) {
        memcpy(new_table->bins, old_table->bins, sizeof(struct st_table_entry *) * old_table->num_bins);
        return new_table;
    }

    if ((ptr = old_table->head) != 0) {
	prev = 0;
	tail = &new_table->head;
	do {
	    entry = alloc(st_table_entry);
	    if (entry == 0) {
		st_free_table(new_table);
		return 0;
	    }
	    *entry = *ptr;
	    hash_val = entry->hash % num_bins;
	    entry->next = new_table->bins[hash_val];
	    new_table->bins[hash_val] = entry;
	    entry->back = prev;
	    *tail = prev = entry;
	    tail = &entry->fore;
	} while ((ptr = ptr->fore) != 0);
	new_table->tail = prev;
    }

    return new_table;
}