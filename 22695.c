st_delete_safe(register st_table *table, register st_data_t *key, st_data_t *value, st_data_t never)
{
    st_index_t hash_val;
    register st_table_entry *ptr;

    if (table->entries_packed) {
	st_index_t i;
	for (i = 0; i < table->num_entries; i++) {
	    if ((st_data_t)table->bins[i*2] == *key) {
		if (value != 0) *value = (st_data_t)table->bins[i*2+1];
		table->bins[i*2] = (void *)never;
		return 1;
	    }
	}
	if (value != 0) *value = 0;
	return 0;
    }

    hash_val = do_hash_bin(*key, table);
    ptr = table->bins[hash_val];

    for (; ptr != 0; ptr = ptr->next) {
	if ((ptr->key != never) && EQUAL(table, ptr->key, *key)) {
	    REMOVE_ENTRY(table, ptr);
	    *key = ptr->key;
	    if (value != 0) *value = ptr->record;
	    ptr->key = ptr->record = never;
	    return 1;
	}
    }

    if (value != 0) *value = 0;
    return 0;
}