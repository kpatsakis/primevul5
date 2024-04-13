static int copyTdEntry(const indexEntry entry, rpmtd td, headerGetFlags flags)
{
    rpm_count_t count = entry->info.count;
    int rc = 1;		/* XXX 1 on success. */
    /* ALLOC overrides MINMEM */
    int allocMem = flags & HEADERGET_ALLOC;
    int minMem = allocMem ? 0 : flags & HEADERGET_MINMEM;
    int argvArray = (flags & HEADERGET_ARGV) ? 1 : 0;

    assert(td != NULL);
    td->flags = RPMTD_IMMUTABLE;
    switch (entry->info.type) {
    case RPM_BIN_TYPE:
	/*
	 * XXX This only works for
	 * XXX 	"sealed" HEADER_IMMUTABLE/HEADER_SIGNATURES/HEADER_IMAGE.
	 * XXX This will *not* work for unsealed legacy HEADER_IMAGE (i.e.
	 * XXX a legacy header freshly read, but not yet unloaded to the rpmdb).
	 */
	if (ENTRY_IS_REGION(entry)) {
	    int32_t * ei = ((int32_t *)entry->data) - 2;
	    entryInfo pe = (entryInfo) (ei + 2);
	    unsigned char * dataStart = (unsigned char *) (pe + ntohl(ei[0]));
	    int32_t rdl = -entry->info.offset;	/* negative offset */
	    int32_t ril = rdl/sizeof(*pe);

	    rdl = entry->rdlen;
	    count = 2 * sizeof(*ei) + (ril * sizeof(*pe)) + rdl;
	    if (entry->info.tag == RPMTAG_HEADERIMAGE) {
		ril -= 1;
		pe += 1;
	    } else {
		count += REGION_TAG_COUNT;
		rdl += REGION_TAG_COUNT;
	    }

	    td->data = xmalloc(count);
	    ei = (int32_t *) td->data;
	    ei[0] = htonl(ril);
	    ei[1] = htonl(rdl);

	    pe = (entryInfo) memcpy(ei + 2, pe, (ril * sizeof(*pe)));

	    dataStart = (unsigned char *) memcpy(pe + ril, dataStart, rdl);

	    rc = regionSwab(NULL, ril, 0, pe, dataStart, dataStart + rdl, 0, 0);
	    /* don't return data on failure */
	    if (rc < 0) {
		td->data = _free(td->data);
	    }
	    /* XXX 1 on success. */
	    rc = (rc < 0) ? 0 : 1;
	} else {
	    td->data = (!minMem
		? memcpy(xmalloc(count), entry->data, count)
		: entry->data);
	}
	break;
    case RPM_STRING_TYPE:
	/* simple string, but fallthrough if its actually an array */
	if (count == 1 && !argvArray) {
	    td->data = allocMem ? xstrdup(entry->data) : entry->data;
	    break;
	}
    case RPM_STRING_ARRAY_TYPE:
    case RPM_I18NSTRING_TYPE:
    {	const char ** ptrEntry;
	int tableSize = (count + argvArray) * sizeof(char *);
	char * t;
	int i;

	if (minMem) {
	    td->data = xmalloc(tableSize);
	    ptrEntry = (const char **) td->data;
	    t = entry->data;
	} else {
	    t = xmalloc(tableSize + entry->length);
	    td->data = (void *)t;
	    ptrEntry = (const char **) td->data;
	    t += tableSize;
	    memcpy(t, entry->data, entry->length);
	}
	for (i = 0; i < count; i++) {
	    *ptrEntry++ = t;
	    t = strchr(t, 0);
	    t++;
	}
	if (argvArray) {
	    *ptrEntry = NULL;
	    td->flags |= RPMTD_ARGV;
	}
    }	break;
    case RPM_CHAR_TYPE:
    case RPM_INT8_TYPE:
    case RPM_INT16_TYPE:
    case RPM_INT32_TYPE:
    case RPM_INT64_TYPE:
	if (allocMem) {
	    td->data = xmalloc(entry->length);
	    memcpy(td->data, entry->data, entry->length);
	} else {
	    td->data = entry->data;
	}
	break;
    default:
	/* WTH? Don't mess with unknown data types... */
	rc = 0;
	td->data = NULL;
	break;
    }
    td->type = entry->info.type;
    td->count = count;
    td->size = entry->length;

    if (td->data && entry->data != td->data) {
	td->flags |= RPMTD_ALLOCED;
    }

    return rc;
}