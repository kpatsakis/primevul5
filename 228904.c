filter_data_internal(struct filter_session *fs, uint64_t token, uint64_t reqid, const char *line)
{
	struct filter_chain	*filter_chain;
	struct filter_entry	*filter_entry;
	struct filter		*filter;

	if (!token)
		fs->phase = FILTER_DATA_LINE;
	if (fs->phase != FILTER_DATA_LINE)
		fatalx("misbehaving filter");

	/* based on token, identify the filter_entry we should apply  */
	filter_chain = dict_get(&filter_chains, fs->filter_name);
	filter_entry = TAILQ_FIRST(&filter_chain->chain[fs->phase]);
	if (token) {
		TAILQ_FOREACH(filter_entry, &filter_chain->chain[fs->phase], entries)
		    if (filter_entry->id == token)
			    break;
		if (filter_entry == NULL)
			fatalx("misbehaving filter");
		filter_entry = TAILQ_NEXT(filter_entry, entries);
	}

	/* no filter_entry, we either had none or reached end of chain */
	if (filter_entry == NULL) {
		io_printf(fs->io, "%s\n", line);
		return;
	}

	/* pass data to the filter */
	filter = dict_get(&filters, filter_entry->name);
	filter_data_query(filter, filter_entry->id, reqid, line);
}