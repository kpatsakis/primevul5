st_init_table_with_size(const struct st_hash_type *type, st_index_t size)
{
    st_table *tbl;

#ifdef HASH_LOG
# if HASH_LOG+0 < 0
    {
	const char *e = getenv("ST_HASH_LOG");
	if (!e || !*e) init_st = 1;
    }
# endif
    if (init_st == 0) {
	init_st = 1;
	atexit(stat_col);
    }
#endif

    size = new_size(size);	/* round up to prime number */

    tbl = alloc(st_table);
    tbl->type = type;
    tbl->num_entries = 0;
    tbl->entries_packed = type == &type_numhash && size/2 <= MAX_PACKED_NUMHASH;
    tbl->num_bins = size;
    tbl->bins = (st_table_entry **)Calloc(size, sizeof(st_table_entry*));
    tbl->head = 0;
    tbl->tail = 0;

    return tbl;
}