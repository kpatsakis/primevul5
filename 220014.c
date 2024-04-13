rpmTagVal headerNextTag(HeaderIterator hi)
{
    indexEntry entry = nextIndex(hi);
    return entry ? entry->info.tag : RPMTAG_NOT_FOUND;
}