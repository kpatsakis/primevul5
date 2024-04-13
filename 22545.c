st_add_direct(st_table *table, st_data_t key, st_data_t value)
{
    st_index_t hash_val, bin_pos;

    if (table->entries_packed) {
        int i;
        if (MORE_PACKABLE_P(table)) {
            i = table->num_entries++;
            table->bins[i*2] = (struct st_table_entry*)key;
            table->bins[i*2+1] = (struct st_table_entry*)value;
            return;
        }
        else {
            unpack_entries(table);
        }
    }

    hash_val = do_hash(key, table);
    bin_pos = hash_val % table->num_bins;
    ADD_DIRECT(table, key, value, hash_val, bin_pos);
}