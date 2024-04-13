st_foreach(st_table *table, int (*func)(ANYARGS), st_data_t arg)
{
    st_table_entry *ptr, **last, *tmp;
    enum st_retval retval;
    st_index_t i;

    if (table->entries_packed) {
        for (i = 0; i < table->num_entries; i++) {
            st_index_t j;
            st_data_t key, val;
            key = (st_data_t)table->bins[i*2];
            val = (st_data_t)table->bins[i*2+1];
            retval = (*func)(key, val, arg);
	    if (!table->entries_packed) goto unpacked;
            switch (retval) {
	      case ST_CHECK:	/* check if hash is modified during iteration */
                for (j = 0; j < table->num_entries; j++) {
                    if ((st_data_t)table->bins[j*2] == key)
                        break;
                }
                if (j == table->num_entries) {
                    /* call func with error notice */
                    retval = (*func)(0, 0, arg, 1);
                    return 1;
                }
		/* fall through */
	      case ST_CONTINUE:
		break;
	      case ST_STOP:
		return 0;
	      case ST_DELETE:
                table->num_entries--;
                memmove(&table->bins[i*2], &table->bins[(i+1)*2],
                        sizeof(struct st_table_entry*) * 2*(table->num_entries-i));
                i--;
                break;
            }
        }
        return 0;
      unpacked:
	ptr = table->head;
	while (i-- > 0) {
	    if (!(ptr = ptr->fore)) return 0;
	}
    }
    else {
	ptr = table->head;
    }

    if (ptr != 0) {
	do {
	    i = ptr->hash % table->num_bins;
	    retval = (*func)(ptr->key, ptr->record, arg);
	    switch (retval) {
	      case ST_CHECK:	/* check if hash is modified during iteration */
		for (tmp = table->bins[i]; tmp != ptr; tmp = tmp->next) {
		    if (!tmp) {
			/* call func with error notice */
			retval = (*func)(0, 0, arg, 1);
			return 1;
		    }
		}
		/* fall through */
	      case ST_CONTINUE:
		ptr = ptr->fore;
		break;
	      case ST_STOP:
		return 0;
	      case ST_DELETE:
		last = &table->bins[ptr->hash % table->num_bins];
		for (; (tmp = *last) != 0; last = &tmp->next) {
		    if (ptr == tmp) {
			tmp = ptr->fore;
			*last = ptr->next;
			REMOVE_ENTRY(table, ptr);
			free(ptr);
			if (ptr == tmp) return 0;
			ptr = tmp;
			break;
		    }
		}
	    }
	} while (ptr && table->head);
    }
    return 0;
}