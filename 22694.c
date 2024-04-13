st_delete(register st_table *table, register st_data_t *key, st_data_t *value)
{
    st_index_t hash_val;
    st_table_entry **prev;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == *key) {
                if (value != 0) *value = (st_data_t)table->bins[i*2+1];
                table->num_entries--;
                memmove(&table->bins[i*2], &table->bins[(i+1)*2],
                        sizeof(struct st_table_entry*) * 2*(table->num_entries-i));
                return 1;
            }
        }
        if (value != 0) *value = 0;
        return 0;
    }

    hash_val = do_hash_bin(*key, table);

    for (prev = &table->bins[hash_val]; (ptr = *prev) != 0; prev = &ptr->next) {
	if (EQUAL(table, *key, ptr->key)) {
	    *prev = ptr->next;
	    REMOVE_ENTRY(table, ptr);
	    if (value != 0) *value = ptr->record;
	    *key = ptr->key;
	    free(ptr);
	    return 1;
	}
    }

    if (value != 0) *value = 0;
    return 0;
}