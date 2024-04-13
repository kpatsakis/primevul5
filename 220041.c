static int intAppendEntry(Header h, rpmtd td)
{
    indexEntry entry;
    int length;

    if (td->type == RPM_STRING_TYPE || td->type == RPM_I18NSTRING_TYPE) {
	/* we can't do this */
	return 0;
    }

    /* Find the tag entry in the header. */
    entry = findEntry(h, td->tag, td->type);
    if (!entry)
	return 0;

    length = dataLength(td->type, td->data, td->count, 0, NULL);
    if (length < 0)
	return 0;

    if (ENTRY_IN_REGION(entry)) {
	char * t = xmalloc(entry->length + length);
	memcpy(t, entry->data, entry->length);
	entry->data = t;
	entry->info.offset = 0;
    } else
	entry->data = xrealloc(entry->data, entry->length + length);

    copyData(td->type, ((char *) entry->data) + entry->length, 
	     td->data, td->count, length);

    entry->length += length;

    entry->info.count += td->count;

    return 1;
}