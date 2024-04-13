static int intAddEntry(Header h, rpmtd td)
{
    indexEntry entry;
    rpm_data_t data;
    int length = 0;

    /* Count must always be >= 1 for headerAddEntry. */
    if (td->count <= 0)
	return 0;

    if (hdrchkType(td->type))
	return 0;
    if (hdrchkData(td->count))
	return 0;

    data = grabData(td->type, td->data, td->count, &length);
    if (data == NULL)
	return 0;

    /* Allocate more index space if necessary */
    if (h->indexUsed == h->indexAlloced) {
	h->indexAlloced += INDEX_MALLOC_SIZE;
	h->index = xrealloc(h->index, h->indexAlloced * sizeof(*h->index));
    }

    /* Fill in the index */
    entry = h->index + h->indexUsed;
    entry->info.tag = td->tag;
    entry->info.type = td->type;
    entry->info.count = td->count;
    entry->info.offset = 0;
    entry->data = data;
    entry->length = length;

    if (h->indexUsed > 0 && td->tag < h->index[h->indexUsed-1].info.tag)
	h->sorted = 0;
    h->indexUsed++;

    return 1;
}