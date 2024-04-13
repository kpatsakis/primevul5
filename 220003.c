int headerNext(HeaderIterator hi, rpmtd td)
{
    indexEntry entry = nextIndex(hi);
    int rc = 0;

    rpmtdReset(td);
    if (entry) {
	td->tag = entry->info.tag;
	rc = copyTdEntry(entry, td, HEADERGET_DEFAULT);
    }
    return ((rc == 1) ? 1 : 0);
}