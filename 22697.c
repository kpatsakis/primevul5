unpack_entries(register st_table *table)
{
    st_index_t i;
    struct st_table_entry *packed_bins[MAX_PACKED_NUMHASH*2];
    st_table tmp_table = *table;

    memcpy(packed_bins, table->bins, sizeof(struct st_table_entry *) * table->num_entries*2);
    table->bins = packed_bins;
    tmp_table.entries_packed = 0;
    tmp_table.num_entries = 0;
    memset(tmp_table.bins, 0, sizeof(struct st_table_entry *) * tmp_table.num_bins);
    for (i = 0; i < table->num_entries; i++) {
        st_insert(&tmp_table, (st_data_t)packed_bins[i*2], (st_data_t)packed_bins[i*2+1]);
    }
    *table = tmp_table;
}