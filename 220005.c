indexEntry findEntry(Header h, rpmTagVal tag, rpm_tagtype_t type)
{
    indexEntry entry;
    struct indexEntry_s key;

    if (h == NULL) return NULL;
    headerSort(h);

    key.info.tag = tag;

    entry = bsearch(&key, h->index, h->indexUsed, sizeof(*h->index), indexCmp);
    if (entry == NULL)
	return NULL;

    if (type == RPM_NULL_TYPE)
	return entry;

    /* look backwards */
    while (entry->info.tag == tag && entry->info.type != type &&
	   entry > h->index) entry--;

    if (entry->info.tag == tag && entry->info.type == type)
	return entry;

    return NULL;
}