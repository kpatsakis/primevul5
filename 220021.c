static int intGetTdEntry(Header h, rpmtd td, headerGetFlags flags)
{
    indexEntry entry;
    int rc;

    /* First find the tag */
    /* FIX: h modified by sort. */
    entry = findEntry(h, td->tag, RPM_NULL_TYPE);
    if (entry == NULL) {
	/* Td is zeroed above, just return... */
	return 0;
    }

    if (entry->info.type == RPM_I18NSTRING_TYPE && !(flags & HEADERGET_RAW))
	rc = copyI18NEntry(h, entry, td, flags);
    else
	rc = copyTdEntry(entry, td, flags);

    if (rc == 0)
	td->flags |= RPMTD_INVALID;

    /* XXX 1 on success */
    return ((rc == 1) ? 1 : 0);
}