st_insert2(register st_table *table, register st_data_t key, st_data_t value,
	   st_data_t (*func)(st_data_t))
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                table->bins[i*2+1] = (struct st_table_entry*)value;
                return 1;
            }
        }
        if (MORE_PACKABLE_P(table)) {
            i = table->num_entries++;
            table->bins[i*2] = (struct st_table_entry*)key;
            table->bins[i*2+1] = (struct st_table_entry*)value;
            return 0;
        }
        else {
            unpack_entries(table);
        }
    }

    hash_val = do_hash(key, table);
    FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
	key = (*func)(key);
	ADD_DIRECT(table, key, value, hash_val, bin_pos);
	return 0;
    }
    else {
	ptr->record = value;
	return 1;
    }
}