st_get_key(st_table *table, register st_data_t key, st_data_t *result)
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                if (result !=0) *result = (st_data_t)table->bins[i*2];
                return 1;
            }
        }
        return 0;
    }

    hash_val = do_hash(key, table);
    FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
	return 0;
    }
    else {
	if (result != 0)  *result = ptr->key;
	return 1;
    }
}