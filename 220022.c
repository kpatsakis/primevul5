static Header headerCreate(void *blob, int32_t indexLen)
{
    Header h = xcalloc(1, sizeof(*h));
    if (blob) {
	h->blob = blob;
	h->indexAlloced = indexLen + 1;
	h->indexUsed = indexLen;
    } else {
	h->indexAlloced = INDEX_MALLOC_SIZE;
	h->indexUsed = 0;
    }
    h->instance = 0;
    h->sorted = 0;
    h->index = xcalloc(h->indexAlloced, sizeof(*h->index));

    h->nrefs = 0;
    return headerLink(h);
}