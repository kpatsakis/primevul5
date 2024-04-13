unsigned headerSizeof(Header h, int magicp)
{
    indexEntry entry;
    unsigned int size = 0;
    int i;

    if (h == NULL)
	return size;

    headerSort(h);

    if (magicp == HEADER_MAGIC_YES)
	size += sizeof(rpm_header_magic);

    size += 2 * sizeof(int32_t);	/* count of index entries */

    for (i = 0, entry = h->index; i < h->indexUsed; i++, entry++) {
	/* Regions go in as is ... */
        if (ENTRY_IS_REGION(entry)) {
	    size += entry->length;
	    /* Reserve space for legacy region tag + data */
	    if (i == 0 && (h->flags & HEADERFLAG_LEGACY))
		size += sizeof(struct entryInfo_s) + entry->info.count;
	    continue;
        }

	/* ... and region elements are skipped. */
	if (entry->info.offset < 0)
	    continue;

	/* Alignment */
	size += alignDiff(entry->info.type, size);

	size += sizeof(struct entryInfo_s) + entry->length;
    }

    return size;
}