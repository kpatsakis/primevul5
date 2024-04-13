rehash(register st_table *table)
{
    register st_table_entry *ptr, **new_bins;
    st_index_t i, new_num_bins, hash_val;

    new_num_bins = new_size(table->num_bins+1);
    new_bins = (st_table_entry**)
	xrealloc(table->bins, new_num_bins * sizeof(st_table_entry*));
    for (i = 0; i < new_num_bins; ++i) new_bins[i] = 0;
    table->num_bins = new_num_bins;
    table->bins = new_bins;

    if ((ptr = table->head) != 0) {
	do {
	    hash_val = ptr->hash % new_num_bins;
	    ptr->next = new_bins[hash_val];
	    new_bins[hash_val] = ptr;
	} while ((ptr = ptr->fore) != 0);
    }
}