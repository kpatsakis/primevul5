st_cleanup_safe(st_table *table, st_data_t never)
{
    st_table_entry *ptr, **last, *tmp;
    st_index_t i;

    if (table->entries_packed) {
	st_index_t i = 0, j = 0;
	while ((st_data_t)table->bins[i*2] != never) {
	    if (i++ == table->num_entries) return;
	}
	for (j = i; ++i < table->num_entries;) {
	    if ((st_data_t)table->bins[i*2] == never) continue;
	    table->bins[j*2] = table->bins[i*2];
	    table->bins[j*2+1] = table->bins[i*2+1];
	    j++;
	}
	table->num_entries = j;
	return;
    }

    for (i = 0; i < table->num_bins; i++) {
	ptr = *(last = &table->bins[i]);
	while (ptr != 0) {
	    if (ptr->key == never) {
		tmp = ptr;
		*last = ptr = ptr->next;
		free(tmp);
	    }
	    else {
		ptr = *(last = &ptr->next);
	    }
	}
    }
}