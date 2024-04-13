static indexEntry nextIndex(HeaderIterator hi)
{
    Header h = hi->h;
    int slot;
    indexEntry entry = NULL;

    for (slot = hi->next_index; slot < h->indexUsed; slot++) {
	entry = h->index + slot;
	if (!ENTRY_IS_REGION(entry))
	    break;
    }
    hi->next_index = slot;
    if (entry == NULL || slot >= h->indexUsed)
	return NULL;

    hi->next_index++;
    return entry;
}